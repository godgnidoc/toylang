import { dark } from "/src/theme";
import { makeIntroduceScene } from "/src/scenes/introduce";
import sections from "./sections.json";
import {
  all,
  chain,
  createRef,
  createRefArray,
  sequence,
  Vector2,
} from "@motion-canvas/core";
import { Circle, Layout, Line, Rect, Txt } from "@motion-canvas/2d";

export default makeIntroduceScene(
  dark,
  "从零开发语法分析器",
  "词法分析 第三节",
  sections,
  function* (this) {
    const scanner = createRef<Rect>();
    this.view.add(
      <Rect
        ref={scanner}
        scale={0}
        layout={true}
        radius={8}
        lineWidth={2}
        padding={32}
      >
        <Txt fontSize={32}>Scanner</Txt>
      </Rect>
    );

    yield* all(scanner().scale(1, 0.8), this.light(scanner, 0.8));
    const lexicon = createRef<Rect>();
    this.view.add(
      <Rect
        ref={lexicon}
        scale={0}
        layout={true}
        radius={8}
        lineWidth={2}
        padding={32}
        top={() => scanner().bottom().addY(128)}
      >
        <Txt fontSize={32}>Lexicon</Txt>
      </Rect>
    );

    yield* all(lexicon().scale(1, 0.8), this.light(lexicon, 0.8));

    const lexiconToScanner = createRef<Line>();
    this.view.add(
      <Line
        ref={lexiconToScanner}
        points={[lexicon().position, scanner().position]}
        lineWidth={2}
        endArrow={true}
        startOffset={() => lexicon().height() / 2}
        endOffset={() => scanner().height() / 2}
        end={0}
      />
    );

    this.light(lexiconToScanner);
    yield* all(
      this.light(lexiconToScanner, 0.8),
      lexiconToScanner().end(1, 0.8)
    );

    const inputSequence = createRef<Layout>();
    this.view.add(
      <Layout
        ref={inputSequence}
        layout={true}
        gap={12}
        right={() => scanner().left().addX(-128)}
      ></Layout>
    );

    for (const char of "int a = 10") {
      const item = createRef<Rect>();

      inputSequence().add(
        <Rect ref={item} scale={0} radius={8} lineWidth={2} padding={4}>
          <Txt fontSize={24}>'{char}'</Txt>
        </Rect>
      );
      this.hide(item);
    }

    const inputSequenceToScanner = createRef<Line>();
    this.view.add(
      <Line
        ref={inputSequenceToScanner}
        points={[inputSequence().position, scanner().position]}
        lineWidth={2}
        endArrow={true}
        startOffset={() => inputSequence().width() / 2}
        endOffset={() => scanner().width() / 2}
        end={0}
      />
    );
    this.light(inputSequenceToScanner);

    yield* chain(
      ...inputSequence()
        .children()
        .map((i) => all(this.light(i, 0.1), i.scale(1, 0.1))),
      inputSequenceToScanner().end(1, 0.4)
    );

    const outputSequence = createRef<Layout>();
    this.view.add(
      <Layout
        ref={outputSequence}
        layout={true}
        gap={12}
        left={() => scanner().right().addX(128)}
      ></Layout>
    );

    for (const token of ["int", "a", "=", "10"]) {
      const item = createRef<Rect>();

      outputSequence().add(
        <Rect ref={item} scale={0} radius={8} lineWidth={2} padding={8}>
          <Txt fontSize={24}>{token}</Txt>
        </Rect>
      );
      this.hide(item);
    }

    const outputSequenceToScanner = createRef<Line>();
    this.view.add(
      <Line
        ref={outputSequenceToScanner}
        points={[scanner().position, outputSequence().position]}
        lineWidth={2}
        endArrow={true}
        startOffset={() => scanner().width() / 2}
        endOffset={() => outputSequence().width() / 2}
        end={0}
      />
    );

    this.light(outputSequenceToScanner);
    yield* chain(
      outputSequenceToScanner().end(1, 0.4),
      ...outputSequence()
        .children()
        .map((i) => all(this.light(i, 0.1), i.scale(1, 0.1)))
    );

    const rules = createRef<Layout>();
    this.view.add(
      <Layout
        ref={rules}
        layout={true}
        direction={"column"}
        gap={12}
        right={() => lexicon().left().addX(-128)}
      ></Layout>
    );
    for (const rule of [
      "INT = /int/",
      "ID = /[a-zA-Z_][a-zA-Z0-9_]*/",
      "ASSIGN = /=/",
      "NUMBER = /[0-9]+/",
      "SPACE = /\\s+/",
    ]) {
      const item = createRef<Rect>();

      rules().add(
        <Rect ref={item} scale={0} radius={8} lineWidth={2} padding={8}>
          <Txt fontSize={24}>{rule}</Txt>
        </Rect>
      );
      this.hide(item);
    }

    const rulesToLexicon = createRef<Line>();
    this.view.add(
      <Line
        ref={rulesToLexicon}
        points={[rules().position, lexicon().position]}
        lineWidth={2}
        endArrow={true}
        startOffset={() => rules().width() / 2}
        endOffset={() => lexicon().width() / 2}
        end={0}
      />
    );

    this.light(rulesToLexicon);
    yield* all(
      chain(
        ...rules()
          .children()
          .map((i) => all(this.light(i, 0.1), i.scale(1, 0.1)))
      ),
      rulesToLexicon().end(1, 0.4),
      lexicon().childAs<Txt>(0).text("Lexicon | DFA", 0.4),
      this.accent(lexicon, 0.4)
    );

    yield* all(
      this.dim(scanner, 0.8),
      this.dim(inputSequence, 0.8),
      this.dim(outputSequence, 0.8),
      this.dim(inputSequenceToScanner, 0.8),
      this.dim(outputSequenceToScanner, 0.8),
      this.dim(lexiconToScanner, 0.8),
      this.outline(0, 0.8)
    );

    yield* all(
      this.dim(rules, 0.8),
      this.dim(rulesToLexicon, 0.8),
      this.dim(lexicon, 0.8),
      this.accent(scanner, 0.8),
      scanner().childAs<Txt>(0).text("Interface | Structure", 0.8),
      this.outline(1, 0.8)
    );

    const comps = createRef<Layout>();
    this.view.add(
      <Layout
        ref={comps}
        layout={true}
        gap={32}
        bottom={() => scanner().top().addY(-32)}
      ></Layout>
    );

    for (const comp of ["Token", "Source", "Builder"]) {
      const item = createRef<Rect>();
      comps().add(
        <Rect ref={item} scale={0} radius={8} lineWidth={2} padding={8}>
          <Txt fontSize={24}>{comp}</Txt>
        </Rect>
      );
      this.hide(item);
    }

    yield* all(
      chain(
        ...comps()
          .children()
          .map((i) => all(this.light(i, 0.1), i.scale(1, 0.1)))
      )
    );

    yield* all(
      scanner().childAs<Txt>(0).text("Scanner", 0.8),
      this.dim(comps, 0.8),
      this.dim(scanner, 0.8),
      this.accent(rules, 0.8),
      this.accent(lexicon, 0.8),
      this.accent(rulesToLexicon, 0.8),
      this.outline(2, 0.8)
    );
    yield* all(
      this.light(scanner, 0.8),
      this.light(lexicon, 0.8),
      this.light(rules, 0.8),
      this.light(rulesToLexicon, 0.8),
      this.light(lexiconToScanner, 0.8),
      this.light(comps, 0.8),
      this.outline(3, 0.8)
    );

    yield* all(
      this.light(inputSequence, 0.8),
      this.light(outputSequence, 0.8),
      this.light(inputSequenceToScanner, 0.8),
      this.light(outputSequenceToScanner, 0.8)
    );

    yield* all(
      this.hide(scanner, 0.8),
      this.hide(lexicon, 0.8),
      this.hide(rules, 0.8),
      this.hide(comps, 0.8),
      this.hide(inputSequence, 0.8),
      this.hide(outputSequence, 0.8),
      this.hide(inputSequenceToScanner, 0.8),
      this.hide(outputSequenceToScanner, 0.8),
      this.hide(lexiconToScanner, 0.8),
      this.hide(rulesToLexicon, 0.8),
      this.choose(0, 2.4)
    );
  }
);
