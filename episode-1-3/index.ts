import { makeProject } from '@motion-canvas/core'

import surface from './chapter-3/0.surface?scene'
import introduce from './chapter-3/1.0.introduce?scene'
import state_machine from './chapter-3/1.1.state_machine?scene'

export default makeProject({
  scenes: [surface, introduce, state_machine],
})
