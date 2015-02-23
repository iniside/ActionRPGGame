Hello and welcome to ActionRPG Game project for Unreal Engine 4.


Goal of this project is to provide sample game framework for creating RPG game elements in any type of game.
There is also sample implementation of how you can intergate modules, to make them work togather, inside
GameSystem module. 

Modules:

ActionRPGGame - main game module, which will implement very game specific parts of code. Like attributes.

GameAbilities - base implementation of Abilities for game. Abilities are simple instanced objects, which contain
state machine.

GameAnimations - nothing yet. In long run it's supposed to contain some basic animation functionality
like IK.

GameAttributes - Most core module of project. Contains implementation of attributes and effects.
Attributes are of two types. Primitive and Complex. Primitive are simple floats, while Complex, are structs,
which can self track their current state.
Effects, are the only safe way to interact with attributes. They implement you standard buff/debuff system
as well as damage system (they are simply categorized as instant effects, fire and forget).

GameInterfaces - an collection of interfaces shared between all modules.

GameSystem - sample itegration of all game modules except ActionRPGGame.

GameTrace - Standard implementation of some tracing objects, which can be easily added to other objects to
let designer easily set tracing (though care must be taken, on how exactly let designer setup tracing,
as those object do not provide any standard implementation, only basic hooks to retrieve data).

GameWeapons - base implementation of weapons. It's very similiar to Abilities, except, Weapons, are 
Actors, can exist in world, and can take care of their own network replication.