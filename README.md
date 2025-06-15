
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
### Why is it impotant?
### Use cases

# How to use the Shrimple Object Pooler 
## Creating an Object Pooler
To initialise

## Getting Objects

## Removing Objects

## Pooling Settings

## Removing a Pooled class

# Testing the Pooler

To test performance of the system, both tests are done within blueprints becuase this .

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
| | **Normal spawning** | **Pooled + pre-spawn** | **Pooled + no pre-spawn** |
| --- | --- | --- | --- |
| Normal Spawning | 40ms |
| Pooled Objects  |
> [!NOTE]
> The **`RED`** line on the graph is the game thread, This is where the spawning will impact.
> The **`GREEN`** line on the graph is the Total frame time.
