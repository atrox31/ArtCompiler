# ACompiler
ArtCompiller is part of the ArtEngine system. This program compiles human readable ArtCode scripts in game objects, just like other compilers.<br>
ArtCode is custom made script language used in ArtCore games. For more usage examples and game making tutorial visit ArtEditor page.<br>
<hr>
## ArtCore
ArtCore is 2D game engine, currently for Windows, later Android and Linux. The whole system consists of 3 components: engine, editor and compilator. For more information about engine or editor go to their repositiries.<br>

## Compilation
ArtCore Editor is created in Visual Studio 2022. There is no cmake file to create new projects.<br>
Simply clone code and compile or download release. There are no non-standard dependencies.<br>
There it is not advisable to run Compiler standalone. Every object must have a specific structure. ArtEditor cares to properly write objects.<br>
For debug step-by-step run with debugger there is a solution. Create a directory "debug_input", insert "AScript.lib" and a custom write object with ".acp" extension.
Run with "-debug" command line option to load from debug_input directory.

## Program usage
This program is running from ArtEditor when the game is compiled. Editor sends two types of objects: Instance definitions and Scene definitions.<br>
Instance definitions contain variables and events. Scene contains global variables, gui (interface) events, level triggers and global triggers.<br>
To compile code it must be written in a special style, more of that later. To run the compilation process open the program with arguments:<br>
* -lib "path_to_lib" <- this is ArtLib path, file is created by ArtCore and contains all functions with arguments that can be run in game.
* -obj "path_to_obj" <- (multiple times) Path to object, every object can be in a separate file or all in one.
* -output "path_to_file" <- If there is no error write all code in one binary file.
* -q <- Quiet work. Do not write log info, only errors.
<br>
If some error while compilation occurs Art Compiler tries to say exactly what and where is wrong. Error syntax:<br>
Error at line: 'code_line_no' in Object: 'object_name' Function: 'function_name' - Message: explain_why_error [in_program_line_number]<br>
In editor this error message transfer to Code editor and select wrong line of code.

# ArtCode
Style of code is like C/Pascal but this is not another code language. This is script language, so syntax can not be complicated. Everything is 
executed in the same frame except delayed code. For more information go to ArtEditor.<br>
Logic and mathematical operators are always 2 chars. You can assign or modify variables by value or function. Arguments in functions can be
another function, there is no limit. There is no way to create custom functions for instances but scenes and levels can have custom triggers.<br>
Script language is case sensitive, type of value is important too. Every variable must be converted to a good type.
In Core there is stack for every variable type so there is no possibility to cast in runtime.

## Operators
Operators work on defined variables. Type variable + operator + value.<br>
Value can be a function. For now there is no option to dynamic create variables.<br>
There is no option to use single char operators like + or -. If there is a need to make an operation
like this use a function. Eg math_sum(a, b)<br>
### Math
'+=' '-=' '*=' '/='
### Assign
':='
### Compare
'||'  '&&' : only bool<br>
'<<'  '>>'  '>='  '<=' : only int, float<br>
'!='  '==' : object, asset, color, string<br>
## Functions
    function(arg0, arg1)
    function(10, 20.8, "text")
    function( function2(10) )
## Conditions
    if( bool )
        ...
    else
        ...
    end
    
    if( value/function/variable == value/function/variable )
        ...
    end

## Types
"null" <- works as NULL, nullptr, void.<br>
"int", "float" <- numeric variables. For float there is normal float precision 0.00000f<br>
"bool" <- true/false/0/1<br>
"instance" <- reference to spawned instance<br>
"object", "sprite", "texture", "sound", "music", "font" <- reference to type, it's expensive to receive but operation is cheap<br>
"point" <- Point structure, have x and y (float). Can be write as x.x:y.y<br>
"rectangle" <- Rectangle structure, function argument tell that it contains x1,y1,x2,y2 or x,y,width,height. It is float type but works with int too.<br>
"color" <- Hex color value, starts with # can be RGB or RGBA. Every value is from 00 to FF (0-255)<br>
"string" <- Normal string variable. Can be 0 length but can not be null.<br>

## Reference (other)
Only in OnCollision events Other token works. Syntax is: other(object_type).variable<br>
This is only possible reference to another object, there is no possible way to call instance.something<br>
Every instance 'see' another type instances variables.<br>
Work in progress.

## Loops and labels
Work in progress.

## Dynamic variables
Variables can be defined only in definition of object.

## Code structure, syntax
Every object must be written in a specific key.<br>
Every function must end with @end, this is exit function singt.<br>
In if`s there are two options: if(condition) ... end or if(condition) ... else ... end<br>
There is no need to write semicons ';'<br>
Code allows you to comment by // in every place in line. There is no way to multiline comment like /* */<br>
Every object definition syntax: object [name] ... @end<br>
Every variable definition syntax: keyword local [type] [name]<br>
Every function must have object_name:function_name<br>
DEF_VALUES is a special event generated by ArtEditor. More for events in ArtEditor documentation<br>
<br>
Example code sample:<br>

    object <object_name>
    object obj_ball
        // variables
        local <type> <name>
        local int Live
        local point Target
        local texture Texture
        // all events in this object
        define EvDraw
        define EvOnViewLeave
        define EvStep
    @end

    function obj_ball:DEF_VALUES
        instance_set_body_circle(12.0f)
        Point := new_point(10,20)
        Texture := assets_get_texture("texture")
        Live := 10
    @end

    function obj_ball:EvDraw
        //EvDraw
        draw_sprite_self()
    @end

    function obj_ball:EvOnViewLeave
        //EvOnViewLeave
        code_execute_trigger("scene&create_ball")
        instance_delete_self()
    @end

    function obj_ball:EvOnCollision
        //EvOnCollision
        if(get_pos_x() >> 10)
            collision_bounce()
            if(collision_get_collider_name() == "obj_enemy")
                Life -= other(obj_enemy).Damage
                instance_delete_other(collision_get_collider())
            end
        end
    @end
