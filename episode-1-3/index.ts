import { makeProject } from '@motion-canvas/core'

import surface from './000.surface?scene'
import introduce from './001.introduce?scene'
import principle from './002.principle?scene'

export default makeProject({
  scenes: [surface, introduce, principle],
})
