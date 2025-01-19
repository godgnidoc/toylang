import {
  Circle,
  CircleProps,
  Line,
  LineProps,
  Node,
  NodeProps,
  signal,
  Txt,
  useScene2D,
} from "@motion-canvas/2d";
import dagre from "@dagrejs/dagre";
import {
  all,
  createRef,
  makeRef,
  PossibleVector2,
  SignalValue,
  SimpleSignal,
  ThreadGenerator,
  useRandom,
} from "@motion-canvas/core";
import { $ } from "/src/signal";
import { Edge, Graph, GraphProps, Vertex } from "/src/components/graph";

export interface LexiconProps extends GraphProps {}
export class Lexicon extends Graph {
  constructor(props: LexiconProps) {
    super(props);
  }

  states() {
    return this.vertexs();
  }

  state(id: SignalValue<number>) {
    return this.vertex($(id).toString());
  }

  transfers() {
    return this.edges();
  }

  transfer(from: SignalValue<number>, to: SignalValue<number>) {
    return this.edge($(from).toString(), $(to).toString());
  }

  addState(id: SignalValue<number>) {
    return this.addVertex(<Vertex id={id.toString()} />);
  }

  addTransfer(
    from: SignalValue<number>,
    to: SignalValue<number>,
    input: SignalValue<number>
  ) {
    return this.addEdge(
      <Edge
        from={this.vertex(from.toString())}
        to={this.vertex(to.toString())}
        label={String.fromCodePoint($(input))}
      />
    );
  }

  setAccept(
    id: SignalValue<number>,
    accept: SignalValue<string>,
    duration?: number
  ) {
    const state = this.state(id);
    const text = state.text();
    if (duration) {
      return all(
        text.text(`${state.id()}\n${$(accept)}`, duration),
        state.outline(true, duration)
      );
    } else {
      text.text(`${state.id()}\n${$(accept)}`);
      state.outline(true);
    }
  }

  focus(id: SignalValue<number>, duration: number) {
    const view = useScene2D().getView();
    const state = this.state(id);
    const dpos = view.middle().sub(state.absolutePosition());
    return this.position(this.position().add(dpos), duration);
  }
}
