import { dark } from "src/theme";
import { makeIntroduceScene } from "src/scenes/introduce";

export default makeIntroduceScene(
  dark,
  "从零开发语法分析器",
  "词法分析 第三节",
  ["有穷状态机简介", "词法分析器实现", "词法分析器生成"],
  function* (this) {
    yield* this.highlight(0);
    yield* this.highlight(1);
    yield* this.highlight(2);
  }
);
