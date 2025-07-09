
![Icon](Resources/Icon128.png)

# Shrimple Object Pooling
**A lightweight simple to use Unreal Engine Plugin**

This is a simple object pooler plugin for Unreal Engine.

### Known Supported Versions
- 5.4.1
- 5.3.1

# Index
- [About Object Pooling](#about-object-pooling)
- [How to Use](#how-to-use-the-shrimple-object-pooler)
- [Testing](#testing-the-pooler)
- [Results](#results)

# About Object Pooling
### What is it?
An object pooler is a design pattern which helps lower the burden on the CPU for spawning objects during gameplay.<br>
This method pre spawns a number of objects at the start of the poolers lifetime and stores them in a list to be activated at a later time.
When the desired object is needed the game will not need to spend frames creating the object, but rather it retrieves the object from the pooler and calls an activation function which is much faster.

A large downside of object pooling is that the memory increases with the number of objects stored, deactivated pooled object still retain space in memory thus slowing down peformance on larger scales.

# How to use the Shrimple Object Pooler 
## Creating an Object Pooler
To initialise the object pooler a class **`BP_ObjectPooler`** has been provided but is not necessary due to the pooler subsystem being avaliable from any script. 
On Begin play, The Object pooler actor class holds a list which is passed into the Object pooler subsystem, this then creates the objects and stores them in presized arrays ready to be grabbed.

## Getting & Returning Objects
There are two functions each for getting and returning objects.
The first is the single object. This will operate on a single object.


## Pooling Settings

## Removing a Pooled class

# Testing the Pooler

| Hardware | Product | 
| --- | --- |
| CPU | Ryzen 7 3800x |
| GPU | Nvidia RTX 3080 |
| RAM | Corsair 16GBx2 3200Mhz|
| SSD | Samsung Evo 970 Plus 512GB|

To test performance of the system, both tests are called from blueprints. <br>
The C++ pooler should have no negligable difference to the blueprint spawn tests in terms of the BP vs C++ argument for performance. <br>
Both are itterated in blueprint, and like the object pooler functions, spawn actor of classs calls a C++ function, specifically from **`UKismetSystemLibrary`** 

### Pooling Tests
- Initialise the Pooler (This will causes an inital lag spike slightly larger than the NonPooled Test)
- Retrieve all objects spawned and activate them into the world
- Wait 2 Seconds for any postload
- Return all objects to the pooler

### Non Pooled Tests
- For int Loop create a number of objects
- Wait 2 Seconds for any postload
- Destroy all spawned objects using a foreach loop

> [!WARNING]
> This test is an extreme scenario due to the volume of actors spawned.
> Each actor is also running a for loop with 100 increments each tick to mimic a heavyweight class

## Results
| | **500 Objects** | **1000 Objects** |**1500 Objects** |
| --- |       --- | ---   | --- |
| Normal Spawning | 67ms  | 128ms | 188ms | 
| Pooled Objects  | <20ms | 50ms | 80ms |
| Pooler Initial Spike | 68ms | 154ms | 228ms |



> [!NOTE]
> The **`RED`** line on the graph is the game thread, This is where the spawning will impact.
> The **`GREEN`** line on the graph is the Total frame time.
