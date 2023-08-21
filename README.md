# Slash
An open-world RPG game that I have developed using Unreal Engine 5 and C++

![HighresScreenshot00001](https://github.com/Th3RoadNotTaken/Slash/assets/34506769/860dd4b4-1511-46c7-a93a-be1795b43c0e)  ![HighresScreenshot00002](https://github.com/Th3RoadNotTaken/Slash/assets/34506769/8d4467c8-fdc8-484b-8e9b-bcc66dd29aff)

## About the project
I've developed this game using Unreal Engine 5.1 and C++ using the latest capabilities of the Engine such as - 
* Enhanced input -
  - Consist of Input Actions and Input Mapping Contexts which can increase the functionality that can be added directly to the Input without having to code for certain nuances.
  - Input Actions can map input as a boolean, 1D vector, or a 2D vector which allows multiple input key bindings to be handled in one Input Action such as movement keys(W,A,S,D)
  - Modifiers can be used in the Input Mapping Context to modify the value of the input to cater to different needs(Keys S and A can be given a modifier Negate which will allow us to add movement in the opposite direction).
  - Triggers can be used in the Input Mapping Context to provide a condition on which the input will be accepted(Interact button can be given an actuation threshold which won't allow the player to spam the button).
* Inverse Kinematics for the character skeleton bones to bring out more realistic movement on different elevations and terrain.
* Groom components for hair, eyebrows, etc 
* IK Rigs to convert animations for one skeleton to another
* Metasounds to randomize sound effects for different interactions.
* Enemies have patrolling, chasing, and attacking states according to how the player decides to interact with them.
* Enemy AI implemented using Pawn Sensing components and Nav Meshes, and motion warping for harder to dodge enemies.
* Breakable actors like various types of pots and ancient artifacts which break due to a transient field being applied by the weapon if the character attacks.
* Box trace when weapons collide with characters to implement weapon damage and generate appropriate reactions when hit.
* Vector cross and dot products to find the weapon hit angle on a target so that the target can react appropriately.
* Animation Blueprints and Montages for the character and all enemies to handle all types of scenarios.
* HUD elements such as Health Bars, Stamina Bars, Treasure, and Soul counts, and different Visual and Sound effects on unique events.
