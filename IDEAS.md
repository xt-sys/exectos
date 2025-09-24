## ExectOS Ideas
This is a list of ideas that migh but not must be realized.

### XTOSKRNL
 - [ ] Implement mechanism for detecting CPU features and checking hardware requirements. If CPU does not meet
       requirements, it should cause a kernel panic before any non-supported instruction is being used.
 - [ ] Finish framebuffer and terminal implementation. Initialization code is already prepared as well as routines for
       clearing the screen and drawing single points. Terminal should be instantiable (should be able to create many
       terminals and switch between them) and work on top of FB. It should define ANSI colors and scrollback buffer.
