import {
  Circle,
  CircleProps,
  Line,
  Node,
  Rect,
  RectProps,
  signal,
  Txt,
  useScene2D,
  View2D,
} from "@motion-canvas/2d";
import {
  all,
  createRef,
  createRefArray,
  Reference,
  SignalValue,
  SimpleSignal,
  ThreadGenerator,
  Vector2,
} from "@motion-canvas/core";
import { ColorTheme } from "/src/theme";

export interface RegexNodeProps extends CircleProps {
  theme: SignalValue<ColorTheme>;
  note?: SignalValue<string>;
  textFill?: SignalValue<string>;
}

export class RegexNode extends Circle {
  @signal()
  public declare readonly theme: SimpleSignal<ColorTheme, this>;

  @signal()
  public declare readonly note: SimpleSignal<string, this>;

  @signal()
  public declare readonly textFill: SimpleSignal<string, this>;

  readonly text = createRef<Txt>();

  constructor(props: RegexNodeProps) {
    let theme = props.theme;
    if (typeof theme === "function") theme = theme();
    if (!props.size) props.size = MIN_REGEX_NODE_SIZE;
    if (!props.stroke) props.stroke = theme.accent;
    if (!props.fill) props.fill = theme.background;
    if (!props.lineWidth) props.lineWidth = 4;
    props.justifyContent = "center";
    props.alignItems = "center";
    super(props);

    let note = props.note;
    if (typeof note === "function") note = note();
    if (!note) note = "";

    let textFill = props.textFill;
    if (typeof textFill === "function") textFill = textFill();
    if (!textFill) textFill = theme.primary;
    this.add(
      <Txt ref={this.text} fill={textFill} fontSize={48}>
        {note}
      </Txt>
    );
    this.size(() =>
      Math.max(MIN_REGEX_NODE_SIZE, this.text().text().length * 32)
    );
  }
}

export type RegexType =
  | "accept"
  | "char"
  | "range"
  | "concat"
  | "union"
  | "kleene"
  | "positive"
  | "optional";
export interface RegexProps extends RectProps {
  theme: SignalValue<ColorTheme>;
  type: SignalValue<RegexType>;
  id: SignalValue<string>;
}
export abstract class Regex extends Rect {
  @signal()
  public declare readonly theme: SimpleSignal<ColorTheme, this>;

  @signal()
  public declare readonly type: SimpleSignal<RegexType, this>;

  @signal()
  public declare readonly id: SimpleSignal<string, this>;

  readonly node = createRef<RegexNode>();

  private isHighlighted = true;

  private static all: { [id: string]: Regex } = {};

  constructor(props?: RegexProps) {
    super({
      ...props,
      justifyContent: props.justifyContent ?? "center",
      alignItems: props.alignItems ?? "start",
      fill: "#0000",
      layout: true,
    });

    Regex.all[this.id()] = this;
  }

  abstract get subs(): Regex[];
  abstract get lines(): Line[];

  get text() {
    return this.node().text().text;
  }

  get root(): Regex {
    let node: Regex = this;
    let ptr: Node = this;
    while (!(ptr.parent() instanceof View2D)) {
      ptr = ptr.parent();
      if (ptr instanceof Regex) node = ptr;
    }
    return node;
  }

  protected addLine(from: Regex, to: Regex, ref: Reference<Line>): void {
    const scene = useScene2D();
    const view = scene.getView();
    const w2l = view.worldToLocal();
    view.add(
      <Line
        ref={ref}
        zIndex={-1}
        points={[
          () => from.node().absolutePosition().transformAsPoint(w2l),
          () => to.node().absolutePosition().transformAsPoint(w2l),
        ]}
        stroke={this.theme().accent}
        lineWidth={4}
      />
    );
  }

  at(path: string): Regex | undefined {
    if (path[0] !== "/") {
      if (this.id() === path) {
        return this;
      }
      return Regex.Find(path);
    }

    let node: Regex = this;
    for (const c of path.slice(1)) {
      const subs = node.subs;
      if (subs.length === 0) return undefined;

      if (subs.length === 1) {
        node = subs[0];
        continue;
      }

      if (c === "0" || c === "l") {
        node = subs[0];
      } else {
        node = subs[1];
      }
    }
    return node;
  }

  highlight(targets: (string | Regex)[], duration: number): ThreadGenerator {
    const ids: string[] = [];
    for (const target of targets) {
      if (typeof target === "string") {
        if (target[0] === "/") {
          ids.push(this.at(target).id());
        } else {
          ids.push(target);
        }
      } else {
        ids.push(target.id());
      }
    }
    this.isHighlighted = ids.length === 0 || ids.includes(this.id());
    const subs = this.subs;
    const lines = this.lines;
    const anim: ThreadGenerator[] = [];

    for (let idx = 0; idx < lines.length; idx++) {
      const sub = subs[idx];
      const line = lines[idx];
      anim.push(sub.highlight(ids, duration));
      if (this.isHighlighted && sub.isHighlighted) {
        anim.push(line.stroke(this.theme().accent, duration));
      } else {
        anim.push(line.stroke(this.theme().dimmed, duration));
      }
    }

    const node = this.node();
    const text = node.text();
    if (this.isHighlighted) {
      anim.push(node.stroke(this.theme().accent, duration));
      anim.push(text.fill(this.theme().primary, duration));
    } else {
      anim.push(node.stroke(this.theme().dimmed, duration));
      anim.push(text.fill(this.theme().dimmed, duration));
    }

    return all(...anim);
  }

  *focus(target: string | Regex, duration: number): ThreadGenerator {
    if (typeof target == "string") {
      target = this.at(target);
      if (!target) return;
    }
    const from = target.node().absolutePosition();
    const to = this.parent().absolutePosition();
    const vec = to.sub(from);
    yield* this.position(this.position().add(vec), duration);
  }

  static From(props: SignalValue<RegexProps>): Regex {
    if (typeof props === "function") props = props();
    const type = typeof props.type === "string" ? props.type : props.type();
    switch (type) {
      case "accept":
        return (<AcceptRegex {...(props as AcceptRegexProps)} />) as Regex;
      case "char":
        return (<CharRegex {...(props as CharRegexProps)} />) as Regex;
      case "range":
        return (<RangeRegex {...(props as RangeRegexProps)} />) as Regex;
      case "concat":
        return (<ConcatRegex {...(props as ConcatRegexProps)} />) as Regex;
      case "union":
        return (<UnionRegex {...(props as UnionRegexProps)} />) as Regex;
      case "kleene":
        return (<KleeneRegex {...(props as KleeneRegexProps)} />) as Regex;
      case "positive":
        return (<PositiveRegex {...(props as PositiveRegexProps)} />) as Regex;
      case "optional":
        return (<OptionalRegex {...(props as OptionalRegexProps)} />) as Regex;
    }
  }

  static Find(id: string): Regex | undefined {
    return Regex.all[id];
  }
}

export interface AcceptRegexProps extends RegexProps {
  type: SignalValue<"accept">;
  tokenId: SignalValue<number>;
  afters: SignalValue<string[]>;
}
export class AcceptRegex extends Regex {
  @signal()
  public declare readonly tokenId: SimpleSignal<number, this>;

  @signal()
  public declare readonly afters: SimpleSignal<string[], this>;

  private readonly afterLines = createRefArray<Line>();

  constructor(props?: AcceptRegexProps) {
    super({ ...props });

    const afterNodes = createRefArray<Regex>();
    for (const after of this.afters()) {
      const regex = Regex.Find(after);
      if (!regex) continue;
      afterNodes.push(regex);
    }

    this.add(
      <RegexNode
        ref={this.node}
        theme={this.theme}
        fill={this.theme().secondary}
        textFill={this.theme().background}
        note={this.tokenId().toString()}
      />
    );

    for (const afterNode of afterNodes) {
      this.addLine(this, afterNode, this.afterLines);
    }

    this.absolutePosition(() => {
      const x =
        afterNodes.reduce(
          (acc, node) => acc + node.node().absolutePosition().x,
          0
        ) / afterNodes.length;
      const y =
        afterNodes
          .map((node) => node.root)
          .map((root) => root.absolutePosition().y + root.height() / 2)
          .sort()[0] +
        REGEX_GAP +
        MIN_REGEX_NODE_SIZE;
      return new Vector2(x, y);
    });
  }

  get subs(): Regex[] {
    return [];
  }
  get lines(): Line[] {
    return this.afterLines;
  }
}

export interface CharRegexProps extends RegexProps {
  type: SignalValue<"char">;
  char: SignalValue<string>;
}
export class CharRegex extends Regex {
  @signal()
  public declare readonly char: SimpleSignal<string, this>;

  constructor(props?: CharRegexProps) {
    super({ ...props });
    this.add(
      <RegexNode
        ref={this.node}
        theme={this.theme}
        note={JSON.stringify(this.char())}
      />
    );
  }

  get subs(): Regex[] {
    return [];
  }
  get lines(): Line[] {
    return [];
  }
}

export type RegexRangeDir = "positive" | "negative";
export interface RangeRegexProps extends RegexProps {
  type: SignalValue<"range">;
  dir: SignalValue<RegexRangeDir>;
  set: SignalValue<string>;
}
export class RangeRegex extends Regex {
  @signal()
  public declare readonly dir: SimpleSignal<RegexRangeDir, this>;

  @signal()
  public declare readonly set: SimpleSignal<string, this>;

  constructor(props?: RangeRegexProps) {
    super({ ...props });
    this.add(
      <RegexNode ref={this.node} theme={this.theme} note={this.set()} />
    );
  }

  get subs(): Regex[] {
    return [];
  }
  get lines(): Line[] {
    return [];
  }
}

export interface ConcatRegexProps extends RegexProps {
  type: SignalValue<"concat">;
  lhs: SignalValue<RegexProps>;
  rhs: SignalValue<RegexProps>;
}
export class ConcatRegex extends Regex {
  @signal()
  public declare readonly lhs: SimpleSignal<RegexProps, this>;

  @signal()
  public declare readonly rhs: SimpleSignal<RegexProps, this>;

  private readonly lhsRegex = createRef<Regex>();
  private readonly rhsRegex = createRef<Regex>();
  private readonly lhsLine = createRef<Line>();
  private readonly rhsLine = createRef<Line>();

  constructor(props?: ConcatRegexProps) {
    super({ ...props });

    this.add([
      Regex.From({
        ...this.lhs(),
        ref: this.lhsRegex,
        marginTop: MIN_REGEX_NODE_SIZE + REGEX_GAP,
        theme: this.theme,
      }),
      <RegexNode ref={this.node} theme={this.theme} />,
      Regex.From({
        ...this.rhs(),
        ref: this.rhsRegex,
        marginTop: MIN_REGEX_NODE_SIZE + REGEX_GAP,
        theme: this.theme,
      }),
    ]);

    this.addLine(this, this.lhsRegex(), this.lhsLine);
    this.addLine(this, this.rhsRegex(), this.rhsLine);
  }

  get subs(): Regex[] {
    return [this.lhsRegex(), this.rhsRegex()];
  }
  get lines(): Line[] {
    return [this.lhsLine(), this.rhsLine()];
  }
}

export interface UnionRegexProps extends RegexProps {
  type: SignalValue<"union">;
  lhs: SignalValue<RegexProps>;
  rhs: SignalValue<RegexProps>;
}
export class UnionRegex extends Regex {
  @signal()
  public declare readonly lhs: SimpleSignal<RegexProps, this>;

  @signal()
  public declare readonly rhs: SimpleSignal<RegexProps, this>;

  private readonly lhsRegex = createRef<Regex>();
  private readonly rhsRegex = createRef<Regex>();
  private readonly lhsLine = createRef<Line>();
  private readonly rhsLine = createRef<Line>();

  constructor(props?: UnionRegexProps) {
    super({ ...props });
    this.add([
      Regex.From({
        ...this.lhs(),
        ref: this.lhsRegex,
        marginTop: MIN_REGEX_NODE_SIZE + REGEX_GAP,
        theme: this.theme,
      }),
      <RegexNode ref={this.node} theme={this.theme} note="|" />,
      Regex.From({
        ...this.rhs(),
        ref: this.rhsRegex,
        marginTop: MIN_REGEX_NODE_SIZE + REGEX_GAP,
        theme: this.theme,
      }),
    ]);

    this.addLine(this, this.lhsRegex(), this.lhsLine);
    this.addLine(this, this.rhsRegex(), this.rhsLine);
  }

  get subs(): Regex[] {
    return [this.lhsRegex(), this.rhsRegex()];
  }
  get lines(): Line[] {
    return [this.lhsLine(), this.rhsLine()];
  }
}

export interface KleeneRegexProps extends RegexProps {
  type: SignalValue<"kleene">;
  sub: SignalValue<RegexProps>;
}
export class KleeneRegex extends Regex {
  @signal()
  public declare readonly sub: SimpleSignal<RegexProps, this>;

  private readonly subRegex = createRef<Regex>();
  private readonly subLine = createRef<Line>();

  constructor(props?: KleeneRegexProps) {
    super({ ...props, direction: "column", alignItems: "center" });
    this.add([
      <RegexNode
        ref={this.node}
        marginBottom={REGEX_GAP}
        theme={this.theme}
        note="*"
      />,
      Regex.From({
        ...this.sub(),
        theme: this.theme,
        ref: this.subRegex,
      }),
    ]);

    this.addLine(this, this.subRegex(), this.subLine);
  }

  get subs(): Regex[] {
    return [this.subRegex()];
  }
  get lines(): Line[] {
    return [this.subLine()];
  }
}

export interface PositiveRegexProps extends RegexProps {
  type: SignalValue<"positive">;
  sub: SignalValue<RegexProps>;
}
export class PositiveRegex extends Regex {
  @signal()
  public declare readonly sub: SimpleSignal<RegexProps, this>;

  private readonly subRegex = createRef<Regex>();
  private readonly subLine = createRef<Line>();

  constructor(props?: PositiveRegexProps) {
    super({ ...props, direction: "column", alignItems: "center" });
    this.add([
      <RegexNode
        ref={this.node}
        marginBottom={REGEX_GAP}
        theme={this.theme}
        note="+"
      />,
      Regex.From({
        ...this.sub(),
        theme: this.theme,
        ref: this.subRegex,
      }),
    ]);

    this.addLine(this, this.subRegex(), this.subLine);
  }

  get subs(): Regex[] {
    return [this.subRegex()];
  }
  get lines(): Line[] {
    return [this.subLine()];
  }
}

export interface OptionalRegexProps extends RegexProps {
  type: SignalValue<"optional">;
  sub: SignalValue<RegexProps>;
}
export class OptionalRegex extends Regex {
  @signal()
  public declare readonly sub: SimpleSignal<RegexProps, this>;

  private readonly subRegex = createRef<Regex>();
  private readonly subLine = createRef<Line>();

  constructor(props?: OptionalRegexProps) {
    super({ ...props, direction: "column", alignItems: "center" });
    this.add([
      <RegexNode
        ref={this.node}
        marginBottom={REGEX_GAP}
        theme={this.theme}
        note="?"
      />,
      Regex.From({
        ...this.sub(),
        theme: this.theme,
        ref: this.subRegex,
      }),
    ]);

    this.addLine(this, this.subRegex(), this.subLine);
  }

  get subs(): Regex[] {
    return [this.subRegex()];
  }
  get lines(): Line[] {
    return [this.subLine()];
  }
}

const REGEX_GAP = 64;
const MIN_REGEX_NODE_SIZE = 128;
