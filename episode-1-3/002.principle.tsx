import {
  all,
  chain,
  createRef,
  createRefArray,
  Reference,
  waitFor,
} from "@motion-canvas/core";
import { makeSectionScene, SectionScene } from "/src/scenes/section";
import { dark, Transparent } from "/src/theme";
import { Latex, Rect, Txt, useScene2D } from "@motion-canvas/2d";
import { Lexicon } from "../compiler/lexicon";
import _ab from "./002.data.json";
import sections from "./sections.json";
import { Notify } from "../compiler/cli";

const ab = _ab as Notify[];

function loadLexicon(ab: Notify[], lexicon: Reference<Lexicon>) {
  const tokens: { [id: number]: string } = {};
  for (const notify of ab) {
    if (notify.$ === "LexiconAddToken") {
      tokens[notify.id] = notify.name;
    } else if (notify.$ === "LexiconAddState") {
      if (notify.id === 0) continue;
      lexicon().addState(notify.id);
    } else if (notify.$ === "LexiconAddTransfer") {
      if (notify.from === 0) continue;
      lexicon().addTransfer(notify.from, notify.to, notify.input);
    } else if (notify.$ === "LexiconSetAccept") {
      lexicon().setAccept(notify.id, tokens[notify.token]);
    }
  }
}

export default makeSectionScene(dark, sections[0], function* (this) {
  const dfa_text = createRef<Txt>();

  this.view.add(<Txt ref={dfa_text} fontSize={64}></Txt>);

  yield* all(this.light(dfa_text, 0.4), dfa_text().text("D F A", 0.4));

  yield* dfa_text().text("Deterministic Finite Automaton", 0.4);
  yield* waitFor(1);
  // yield* introduce.call(this);

  const lexicon = createRef<Lexicon>();
  this.view.add(<Lexicon rankdir={"LR"} ref={lexicon} />);
  loadLexicon(ab, lexicon);
  this.hide(lexicon);

  /**
   * 展开有穷状态机图示
   */
  yield* all(
    dfa_text().text("DFA", 0.4),
    dfa_text().top(this.top(0.25), 0.4),
    this.light(lexicon(), 0.6),
    lexicon().layout(0.8)
  );

  yield* waitFor(1);

  /**
   * 点亮全部状态
   */
  yield* this.dim(lexicon, 0.4);
  yield* all(
    ...lexicon()
      .vertexs()
      .map((v) => this.light(v, 0.4))
  );
  yield* waitFor(1);

  /**
   * 点亮全部字符输入
   */
  yield* this.dim(lexicon, 0.4);
  yield* all(
    ...lexicon()
      .transfers()
      .map((t) => this.light(t.text, 0.4))
  );
  yield* waitFor(1);

  /**
   * 点亮全部转移
   */
  yield* this.dim(lexicon, 0.4);
  yield* all(
    ...lexicon()
      .transfers()
      .map((t) => this.light(t, 0.4))
  );
  yield* waitFor(1);

  /**
   * 点亮开始状态
   */
  yield* this.dim(lexicon, 0.4);
  yield* all(
    this.light(lexicon().state(1), 0.4)
  );
  yield* waitFor(1);

  /**
   * 点亮接受状态
   */
  yield* this.dim(lexicon, 0.4);
  yield* all(
    this.light(lexicon().state(4), 0.4)
  );
  yield* waitFor(1);

  // /**
  //  * 分析过程演示
  //  */
  // const inputs = createRefArray<Txt>();
  // const working = createRef<Rect>();
  // this.view.add(
  //   <Rect
  //     ref={working}
  //     position={this.bottom(0.25)}
  //     layout={true}
  //     alignItems={"center"}
  //   />
  // );
  // "ababb".split("").forEach((input) => {
  //   working().add(<Txt ref={inputs} fontSize={64} text={input} />);
  // });
  // yield* all(...inputs.map((input) => input.fill(this.theme.primary, 0.8)));

  // const states = [1, 2, 3, 2, 3, 4];
  // let last = states[0];
  // let index = -1;
  // for (let next of states) {
  //   yield* all(
  //     lexicon().highlight([last, next], 0.4),
  //     ...inputs.map((input, i) => {
  //       if (i === index) {
  //         return all(
  //           input.fill(this.theme.primary, 0.4),
  //           input.fontSize(80, 0.4)
  //         );
  //       } else {
  //         return all(
  //           input.fill(this.theme.dimmed, 0.4),
  //           input.fontSize(64, 0.4)
  //         );
  //       }
  //     })
  //   );
  //   yield* lexicon().highlight(next, 0.4);
  //   last = next;
  //   index++;
  // }

  yield* waitFor(1);
});
