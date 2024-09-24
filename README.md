# DX11-Graphics-Programming

This is a demonstration of my small graphics scene for the CMP301 Graphics Programming module, using C++, DirectX11 and HLSL. This showcases a Tessellated Displacement Map, with mathematically calculated Lighting Normals (Both per-vertex and per-pixel). T
This is my CMP301 Graphics Programming module, using C++ DirectX11.

![image](https://user-images.githubusercontent.com/78039370/213821361-5cc9ca1d-2474-49ad-87a4-ff8caa419cc6.png)

## Techniques Demonstracted
The primary techniques demonstrated are:
- Tessellation through a slider
  - Breaking down geometry into more or less triangles to increase graphical fidelity at a performance cost, or decrease graphical fidelity for a performance gain.
  - Primarily used to decrease geometry when being viewed from a distance, as the graphical fidelity difference will be less noticeable from a difference.
- Displacement Map
  - Each vertex of the mountain determines their height based on how bright a pixel is on the displacement map.
- Mathematically Calculated Lighting Normals
  - Per Vertex calculations by crossing the resulting vectors of the positions of the nearby vertices.
  - Per Pixel calculations can be toggled instead, resulting in better fidelity when viewed from a distance but very noticeable up close. Known as "Bump Mapping".
These are demonstrated through the use of a Directional, Spot and Point light respectively. The Spot and Directional light cast Shadows onto the Displacement Map. There is a Depth of Field post process that can be toggled too, to effectively showcase rendering the screen to a texture and manipulating it dynamically. 

## Links
Link to the EXE: https://drive.google.com/drive/folders/15Df9Qgj5DJfPIyg2SuL55Ld3-C15ebtz?usp=share_link
![image](https://user-images.githubusercontent.com/78039370/213821440-fb325804-a647-4da5-801b-4cd64b5d793f.png)


