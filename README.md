# ACompiler
ArtScript is C like language. Have normal operators and types. You can write code in ArtEditor as normal code or click in CodeEditor.
## Usage
ACompiler.exe -lib C:\\AScript.lib -obj C:\\object_definition1.asc  -obj C:\\object_definition2.asc -output C:\\compile.acp
* -lib path to libray with functions
* -obj path to code files
* -output compiler object output

# ArtCode
## Syntax
int a = 10;
point p = NewPoint(10, 20);
start function with function object_name:Event_name
...
@end
## Types
"null", "int", "float", "bool", "instance", "object", "sprite", "texture", "sound", "music", "font", "point", "rectangle", "color", "string"

## Example
object test_obj
local float x
local float y
local texture my_tex
local point Target
local bool moving
function EV_ONCREATE
function EV_DRAW
function EV_STEP
function EV_ONMOUSE_DOWN
@end

function test_obj:DEF_VALUES
x = 0.0f
y = 0.0f
direction = 0.0f
self_sprite = null
mask = null
@end

function test_obj:EV_ONCREATE
Target = new_point(x,y)
my_tex = get_texture("res_lumberman")
@end

function test_obj:EV_DRAW
draw_texture(my_tex,x,y)
@end

function test_obj:EV_STEP
if(moving)
 move_to_point(Target, 100.0f)
  if(distance_to_point(Target) << 8)
   moving = false
  end
 end
@end

function test_obj:EV_ONMOUSE_DOWN
moving = true
Target = get_global_mouse()
@end

 - this make object that draw texture and follow mouse clicks on map
 
