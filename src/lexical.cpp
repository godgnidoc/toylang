#include "toylang/lexical.h"

#include <stdexcept>

namespace toylang {

namespace {

struct State {
  std::optional<int> accept;
  std::map<int, int> transfer;
};

}  // namespace

struct Lexicon::Impl {
  /**
   * 词法记号表，第一个元素ID为1
   */
  std::vector<std::string> tokens_;

  /**
   * 上下文表，第一个元素ID为0
   */
  std::vector<std::string> contexts_;

  /**
   * 状态表
   */
  std::vector<State> states_;
};

Lexicon::Lexicon(std::unique_ptr<Impl>&& impl) : impl_(std::move(impl)) {}

std::string Lexicon::NameOfToken(int token) const {
  if (token == 0)
    return "<EOF>";
  else if (token < 0)
    return "<ERR>";
  else
    return impl_->tokens_.at(token - 1);
}

int Lexicon::IdOfToken(std::string const& name) const {
  for (size_t off = 0; off < impl_->tokens_.size(); off++) {
    if (impl_->tokens_.at(off) == name) return off + 1;
  }

  return Token::kError;
}

std::vector<std::string> Lexicon::ListTokens() const { return impl_->tokens_; }

int Lexicon::CountTokens() const { return impl_->tokens_.size(); }

std::string Lexicon::NameOfContext(int id) const {
  return impl_->contexts_.at(id);
}
int Lexicon::IdOfContext(std::string const& name) const {
  for (size_t id = 0; id < impl_->contexts_.size(); id++) {
    if (impl_->contexts_.at(id) == name) return id;
  }
  throw std::runtime_error("context not found");
}

std::vector<std::string> Lexicon::ListContexts() const {
  return impl_->contexts_;
}

std::optional<int> Lexicon::AcceptOfState(int state) const {
  return impl_->states_.at(state).accept;
}

std::optional<int> Lexicon::TransferOfState(int state, int input) const {
  auto& transfer = impl_->states_.at(state).transfer;
  auto it = transfer.find(input);
  if (it == transfer.end()) return std::nullopt;
  return it->second;
}

Scanner::Scanner()
    : context_{0},
      line_{1UL},
      column_{1UL},
      offset_{0UL},
      lexicon_{nullptr},
      source_{nullptr} {}

Token Scanner::NextToken() {
  if (!lexicon_) throw std::runtime_error("lexicon not set");
  if (!source_) throw std::runtime_error("source not set");

  Token token{
      .id = Token::kEOF,
      .start_line = line_,
      .start_column = column_,
      .end_line = line_,
      .end_column = column_,
      .offset = offset_,
      .length = 0,
      .source = source_,
      .lexicon = lexicon_,
  };

  if (offset_ >= source_->content.size()) return token;

  auto state = lexicon_->TransferOfState(0, context_).value();
  while (state != 0) {
    auto const ch = static_cast<unsigned char>(source_->content[offset_]);
    auto tr = lexicon_->TransferOfState(state, ch);
    if (tr) {
      state = *tr;
    } else if (auto accept = lexicon_->AcceptOfState(state); accept) {
      token.id = *accept;
      break;
    } else {
      token.id = Token::kError;
      if (ch == 0) break;

      state = 0;
    }

    token.length++;
    token.end_line = line_;
    token.end_column = column_;

    offset_++;
    column_++;
    if (ch == '\n') {
      line_++;
      column_ = 1UL;
    }
  }

  return token;
}

void Scanner::SetLexicon(std::shared_ptr<Lexicon const> lexicon) {
  lexicon_ = lexicon;
  context_ = 0;
}
void Scanner::SetSource(std::shared_ptr<Source const> source) {
  source_ = source;
  line_ = 1;
  column_ = 1;
  offset_ = 0;
}
void Scanner::SetContext(int context) { context_ = context; }
void Scanner::SetContext(std::string const& context) {
  context_ = lexicon_->IdOfContext(context);
}

struct Lexicon::Builder::Building {
  /**
   * 对上下文没有要求的正则模式
   */
  std::set<Regex> global_patterns_;

  /**
   * 全文正则表达式
   */
  Regex regex_;

  /**
   * firstpos与上下文的映射
   */
  std::map<std::shared_ptr<regex::LeafNode>, std::set<int>> firstpos_ctx_map_;

  /**
   * 构建的词法规则
   */
  std::unique_ptr<Lexicon::Impl> impl_;

  int TouchContext(std::string const& name) {
    for (size_t id = 0; id < impl_->contexts_.size(); id++)
      if (impl_->contexts_.at(id) == name) return id;

    impl_->contexts_.push_back(name);
    return impl_->contexts_.size() - 1;
  }

  std::set<int> TouchContexts(std::set<std::string> const& names) {
    std::set<int> ids;
    for (auto const& name : names) {
      ids.insert(TouchContext(name));
    }
    return ids;
  }

  int AddToken(std::string const& name) {
    for (auto& it : impl_->tokens_) {
      if (it == name) {
        throw std::runtime_error{"Token " + name + " already exists"};
      }
    }
    impl_->tokens_.push_back(name);
    return impl_->tokens_.size();
  }
};

Lexicon::Builder::Builder() {
  building_ = std::make_unique<Building>();
  building_->impl_ = std::make_unique<Lexicon::Impl>();
  building_->impl_->contexts_.emplace_back(kDefaultContext);
}

Lexicon::Builder::~Builder() {}

Lexicon::Builder& Lexicon::Builder::DefineToken(
    std::string const& name, Regex pattern,
    std::optional<std::set<std::string>> const& context) {
  int token_id = building_->AddToken(name);

  (void)regex::Accept(pattern, token_id);

  if (context && !context->empty()) {
    auto ctx_ids = building_->TouchContexts(*context);

    for (auto const& it : pattern->GetFirstpos())
      building_->firstpos_ctx_map_[it] = ctx_ids;
  } else {
    building_->global_patterns_.insert(pattern);
  }

  if (building_->regex_ == nullptr) {
    building_->regex_ = pattern;
  } else {
    building_->regex_ = regex::Union(building_->regex_, pattern);
  }

  return *this;
}

std::shared_ptr<Lexicon> Lexicon::Builder::Build() {
  auto& regex_ = building_->regex_;
  auto& firstpos_ctx_map_ = building_->firstpos_ctx_map_;

  // 补全全局正则表达式的firstpos与上下文的映射
  for (auto const& it : building_->global_patterns_) {
    for (auto const& firstpos : it->GetFirstpos()) {
      auto& map = firstpos_ctx_map_[firstpos];
      for (auto i = building_->impl_->contexts_.size(); i > 0; --i)
        map.insert(i - 1);
    }
  }

  std::vector<int> pending_states;
  std::map<int, regex::LeafNodes> state_pos_map;

  {
    // 起始状态
    building_->impl_->states_.emplace_back();

    // 计算全部位置的followpos
    regex_->CalcFollowpos();

    // 每个上下文拥有一个首位置状态，包含当前上下文能接受的全部首位置
    for (size_t ctxid = 0; ctxid < building_->impl_->contexts_.size();
         ctxid++) {
      // 创建首状态
      auto const stateid = building_->impl_->states_.size();
      pending_states.push_back(stateid);
      building_->impl_->states_.emplace_back();

      // 对起始状态来说上下文id被用作输入
      building_->impl_->states_.front().transfer.emplace(ctxid, stateid);

      // 计算当前上下文能接受的首位置
      for (auto const& posit : regex_->GetFirstpos()) {
        auto it = firstpos_ctx_map_.find(posit);
        if (it != firstpos_ctx_map_.end() && 0 == it->second.count(ctxid))
          continue;

        state_pos_map[stateid].insert(posit);
      }
    }
  }

  // 处理尚未处理的状态
  while (!pending_states.empty()) {
    // 收集当前状态信息
    auto const state_id = pending_states.back();
    auto const& current_pos = state_pos_map.at(state_id);

    // 将状态标记为已处理
    pending_states.pop_back();

    // 计算当前状态接受的词法记号
    for (auto const& posit : current_pos) {
      if (auto accept = std::dynamic_pointer_cast<regex::AcceptNode>(posit);
          accept != nullptr) {
        auto& state = building_->impl_->states_.at(state_id);
        // 词法记号ID越小，优先级越高
        if (!state.accept || accept->token_id_ < state.accept) {
          state.accept = accept->token_id_;
        }
      }
    }

    // 计算当前状态的出度转移
    for (auto ch = 1; ch <= 255; ch++) {
      regex::LeafNodes followpos;

      // 收集当前输入字符能到达的所有位置
      for (auto it : current_pos) {
        if (!it->Match(ch)) continue;

        followpos.insert(it->followpos_.begin(), it->followpos_.end());
      }

      // 若当前输入字符不能到达任何位置，则跳过
      if (followpos.empty()) continue;

      // 计算当前输入字符能到达的状态
      int next_state_id = 0;
      for (auto const& [candidate_state_id, candidate_state_pos] :
           state_pos_map) {
        if (candidate_state_pos != followpos) continue;

        next_state_id = candidate_state_id;
        break;
      }

      // 若当前输入字符能到达的状态尚未创建，则创建之
      if (next_state_id == 0) {
        next_state_id = building_->impl_->states_.size();
        pending_states.push_back(next_state_id);
        building_->impl_->states_.emplace_back();
        state_pos_map.emplace(next_state_id, followpos);
      }

      // 添加转移
      building_->impl_->states_.at(state_id).transfer.emplace(ch,
                                                              next_state_id);
    }
  }

  auto lexicon = std::make_shared<Lexicon>(std::move(building_->impl_));
  building_.reset();
  return lexicon;
}

}  // namespace toylang