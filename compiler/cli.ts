import { AcceptRegexProps, ConcatRegexProps, RegexProps, UnionRegexProps } from "./regex"


export type NotifyType =
  'RegexCompile' | 'RegexAccept' | 'RegexUnion'
  | 'LexiconAddToken' | 'LexiconAddState' | 'LexiconAddTransfer' | 'LexiconSetAccept'
  | 'ScannerSetSource' | 'ScannerSetState' | 'ScannerNextInput' | 'ScannerNextLine' | 'ScannerAcceptToken'

export interface BasicNotify {
  $: NotifyType
}

export interface RegexCompileNotify extends BasicNotify {
  $: 'RegexCompile'
  pattern: string
  regex: RegexProps
}

export interface RegexAcceptNotify extends BasicNotify {
  $: 'RegexAccept'
  accept: AcceptRegexProps
}

export interface RegexUnionNotify extends BasicNotify {
  $: 'RegexUnion'
  union: string
  left: string
  right: string
}

export interface LexiconAddTokenNotify extends BasicNotify {
  $: 'LexiconAddToken'
  id: number
  name: string
}

export interface LexiconAddStateNotify extends BasicNotify {
  $: 'LexiconAddState'
  id: number
  poses: string[]
}

export interface LexiconAddTransferNotify extends BasicNotify {
  $: 'LexiconAddTransfer'
  from: number
  to: number
  input: number
}

export interface LexiconSetAcceptNotify extends BasicNotify {
  $: 'LexiconSetAccept'
  id: number
  token: number
}

export interface ScannerSetSourceNotify extends BasicNotify {
  $: 'ScannerSetSource'
  source: string
}

export interface ScannerSetStateNotify extends BasicNotify {
  $: 'ScannerSetState'
  state: number
}

export interface ScannerNextInputNotify extends BasicNotify {
  $: 'ScannerNextInput'
}

export interface ScannerNextLineNotify extends BasicNotify {
  $: 'ScannerNextLine'
}

export interface Token {
  id: number
  name: string
  offset: number
  length: number
  start_line: number
  start_column: number
  end_line: number
  end_column: number
  text: string
}

export interface ScannerAcceptTokenNotify extends BasicNotify {
  $: 'ScannerAcceptToken'
  token: Token
}

export type Notify = RegexCompileNotify | RegexAcceptNotify | RegexUnionNotify
  | LexiconAddTokenNotify | LexiconAddStateNotify | LexiconAddTransferNotify | LexiconSetAcceptNotify
  | ScannerSetSourceNotify | ScannerSetStateNotify | ScannerNextInputNotify | ScannerNextLineNotify | ScannerAcceptTokenNotify