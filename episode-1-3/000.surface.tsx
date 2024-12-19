import { dark } from "src/theme";
import { makeSurfaceScene, SurfaceScene } from "/src/scenes/surface";
import { waitUntil } from "@motion-canvas/core";

export default makeSurfaceScene(
  dark,
  "从零开发语法分析器",
  "词法分析 第三节",
  function* (this: SurfaceScene) {
    yield* waitUntil("leaveSurface");
    yield* this.leave();
  }
);
