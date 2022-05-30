# Vlad Oancea's Assignment - the Lightweight-OpenGL-Loader software (L.O.L.)

## Project Summary
My projects consists of a real-time 3D PBR render engine, able to interactively load and position meshes through an interface, assign them textures and display them using multiple types of lights or HDR environment maps and custom backgrounds. The lighting system supports multiple shadow-casting lights, reaching up to 4 point shadows and 8 directional shadows.
The software has Scene Serialization capabilities, being able to save scene files using the JSON format, as well as load them back in. Do be warned that the resources are saved by their absolute filepath, so changing a resource's directory will cause it to be unable to load.
The program allows you to render the resulted composition as a PNG, either by instantly capturing the current viewport as-is, or allowing you to specify a custom resolution going up to 4096x4096.

This could have the practical use of having a tool to quickly showcase models and textures, for either lookdev or personal projects, similar to Marmoset Toolbag, but as a small and lightweight software that renders purely in real-time.

##Implementation Details
#Model Loading

I used NGL's default Obj Loader class as a reference and extended it to be able to support both triangular and quad meshes. To achieve this, I had to figure out a way to store triangulate input meshes on the fly, so I started looking into methods that would allow me to achieve this.
A rather efficient and simple way to approach this would be to triangulate quads based on their shortest diagonal, which was I attempted first, however despite the simplicity of this algorithm, the results were unexpected and the outputs would turn out inconsistent. I arrived to the conclusion that winding-order might be the cause of it, so I started looking for an approach that would be order-independent of the vertices.
The solution I arrived to was polygon-center-triangulation. For each quad, I add an extra vertex in its center, with its UV and Normal averaged from the other 4 vertices, and then triangulate it with the quad's existing vertices. This method managed to produce consistent and high-quality results, so I decided to keep it.

#PBR Shading
After getting my mesh loading functionality in, I started looking into setting up the PBR shader system. My main source of learning was the [LearnOpenGL website](https://learnopengl.com/PBR/Lighting) , which has very thorough explanations about the mathematics and physics behind the methods used in PBR, alongside code examples and walkthroughs on how to configure it. I implemented directional and point lights in my code, and then proceeded to write and implement the shader, adding functionality for dynamic number of lights by rewriting and recompiling .glsl files at runtime. I would later return to this shader when implementing shadows and environment lighting.

#Mouse Picking
I added mouse picking functionality by using a second color attachment to my framebuffer, which only holds integers, and storing the Object ID through the fragment shader. I then read from the buffer at the mouse position whenever I want to know which object I am hovering over.

#Gizmos
One feature I wanted to implement as a quality of life addition was a transformation gizmo that would allow the user to easily manipulate selected objects. I couldn't find many online sources on how to approach this, but after collecting information from various scattered sources, I decided I would have my gizmo as a mesh that I procedurally create at the start of the program, and then draw it on top of everything else when an object is selected, using mouse picking to detect when I am clicking on it. Most solutions I've found online use raycasting from the mouse to detect collision with the gizmo, but since I already the picking system, I decided to integrate it into it.
I had a few problems with configuring the rotation to work on local axis, as the rotation matrix multiplication ordered made the axis dependent on eachother, so I implemented my own RotateByAxis function that makes use of quaternions.

#GUI
I designed a layout using QT Designer and integrated my existing scene into it, hooking up a simple transformation widget to my objects and using a ListView as a scene outliner. This process was rather straightforward, however I had a few issues with building QT widgets and layouts on the fly in order to display the properties of each type of individual object.
