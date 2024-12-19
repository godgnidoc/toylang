import { all, waitFor } from "@motion-canvas/core";
import { makeThemedScene, ThemedScene } from "src/scenes/themed";
import { dark } from "src/theme";

import ab from "/data/data_ab.json";
import { RegexCompileNotify } from "src/scenes/compiler/cli";
import { Regex } from "src/scenes/compiler/regex";

export default makeThemedScene(dark, function* (this: ThemedScene) {
  const notify = ab.find((v) => v.$ === "RegexCompile") as RegexCompileNotify;
  const regex = Regex.From({ ...notify.regex, theme: this.theme });
  this.view.add(regex);

  const highlights: string[] = [];
  yield* regex.focus("/", 0);

  for (let node of ["/10000", "/10001", "/1000"]) {
    highlights.push(node);
    yield* all(regex.highlight(highlights, 1), regex.focus(node, 1));
  }

  highlights.splice(0, highlights.length);
  for (let node of ["/010", "/01"]) {
    highlights.push(node);
    yield* all(regex.highlight(highlights, 1), regex.focus(node, 1));
  }

  yield* all(regex.highlight([], 1), regex.position(0, 1));
  yield* waitFor(1);
});
