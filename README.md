# Slash
An open-world RPG game which I am currently developing using Unreal Engine 5 and C++

![HighresScreenshot00001](https://github.com/Th3RoadNotTaken/Slash/assets/34506769/860dd4b4-1511-46c7-a93a-be1795b43c0e)  ![HighresScreenshot00002](https://github.com/Th3RoadNotTaken/Slash/assets/34506769/8d4467c8-fdc8-484b-8e9b-bcc66dd29aff)

## About the project
I'm developing this game using Unreal Engine 5.1 and C++ using the latest capabilities of the Engine such as - 
* Enchanced input -
  - Consist of Input Actions and Input Mapping Contexts which can increase the functionality that can be added directly to the Input without having to code for certain nuances.
  - Input Actions can map input as a boolean, 1D vector or a 2D vector which allows multiple input key bindings to be handled in one Input Action such as movement keys(W,A,S,D)
  - Modifiers can be used in the Input Mapping Context to modify the value of the input to cater to different needs(Keys S and A can be given a modifier Negate which will allow us to add movement in the opposite direction).
  - Triggers can be used in the Input Mapping Context to provide a condition on which the input will be accepted(Interact button can be given an actuation threshold which won't allow the player to spam the button).
* Inverse Kinematics for the character skeleton bones to bring out more realistic movement on different elevations and terrain.
* Groom components for hair, eyebrows etc 
* IK Rigs to convert animations for one skeleton to another
* Metasounds to randomize sound effects for different interactions.

I'm currently working on weapon hit react animations on the enemy which will enable the enemy to react to the player's attack according to the direction of the hit.
