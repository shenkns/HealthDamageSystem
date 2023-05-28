# Health Damage System

## Description

This plugin is designed to create damage interactions with multiple health objects per component. Better than Unreal's default damage system because use PlayerState instead of PlayerController as instigator of damage(replicated to all clients, but PlayerController is only to owner) and UObject as damage Causer instead of Actor(wider range of supported classes). 

## Dependencies

* [Log System](https://github.com/shenkns/LogSystem.git)
* [Data System](https://github.com/shenkns/DataSystem.git)

## Setup

1) Install dependencies plugins
2) Install this plugin
3) Add ```HealthComponent``` to your character