# Zgame
Gameplay:

- Starting the game:
    + You start in safe room w/ broken portal gun and some money?
        * Locker storage
        * Merchant - Could be randomized items
        * Crafting bench / Work station - Weapon mods, perks, etc.
        * Portal to new location - Starts the round
    + Start in neutral location w/ broken portal gun
        * All enemies are visible but passive
        * Portal to new location - Starts the round
    + Game Seed
        * Random between all players
        * Same seed for all players

- Gameplay:
    + Player walks through portal and spawns in a random environment.
        * Environment randomization is the same for each player but each player may
        exists in an alternate dimension of the same stage
        * Enemy, loot, etc. may be randomized between players if randomized seed was
        the game option
        * Spawning after entering a portal:
            1. Spawn with teammate
            2. Spawn with member of another team
            3. Spawn with everyone in the same location
    + Charging the portal gun on each stage:
        * Kill a certain enemy type?
        * Complete some tasks? 
    + Farming:
        * Enemies drop money that can be collected (Geometry Wars green crystals)
        * Care packages drop with loot or chest spawn on map?
        * Enemies drop special items?
    + Portal Jumping:
        * Players can jump forward to progress towards boss
        * Players can jump laterally to invade adjacent world
        * Enemy portals cannot be shared but team portals can (Otherwise invasion would
        be the priority)



- Gameplay Map:
    + Two Teams of Two, Versus
    + Players can be on any of the parrallel paths from the Start, options below:
        * | Team1Player1 | Team1Player2 | Team2Player1 | Team2Player2 |
        * | T1P1 | T2P1 | T1P2 | T2P2 |
        * | T1P1 + T1P2 | T2P1 + T2P2 | Empty | Empty |
        * | T1P1 + T2P1 | T1P2 + T2P2 | Empty | Empty |
        * | T1P1 + T1P2 + T2P1 + T2P2 | Empty | Empty | Empty |
    + Players can jump laterally (Left or Right Portal w/ wrap around)
        * To Invade Enemy
        * To Link up with teammate
        * All itermediate portals look the same to team members
        * Enemy portals are a different color
        * If an enemy portal shows up on the left side of your stage a right
        portal could either take you to your teammate or the enemy teammate, or
        you could bail into the next world with your own forward portal.
    + Players can jump forward
        * Progress to boss room
        * Forward jumps can take you to alternate path? Maybe but I think that
        would discourage the mid game team up. An team linked could more easily
        eliminate a solo player.
    + Portals are always open
        * All non-boss portals can be returned through
        * Should the shop portal be reaccessible? It would slow you down to
        to return to the very beginning. If so shop inventory 
    + One boss room per team
        * Different Color portal
```
                     #########
                     #       #
                     # START #
                     #       #
                     #########
     _________________________________________
    |             |             |             |
    v             v             v             v
#########     #########     #########     #########
#       #     #       #     #       #     #       #
# JUMP1 # <-> # JUMP1 # <-> # JUMP1 # <-> # JUMP1 #
#       #     #       #     #       #     #       #
#########     #########     #########     #########
    ^     \ /     ^     \ /     ^     \ /     ^
    |      X      |      X      |      X      |
    v     / \     v     / \     v     / \     v
#########     #########     #########     #########
#       #     #       #     #       #     #       #
# JUMP2 # <-> # JUMP2 # <-> # JUMP2 # <-> # JUMP2 #
#       #     #       #     #       #     #       #
#########     #########     #########     #########
    ^     \ /     ^     \ /     ^     \ /     ^
    |      X      |      X      |      X      |
    v     / \     v     / \     v     / \     v
#########     #########     #########     #########
#       #     #       #     #       #     #       #
# JUMP3 # <-> # JUMP3 # <-> # JUMP3 # <-> # JUMP3 #
#       #     #       #     #       #     #       #
#########     #########     #########     #########
    |             |             |             |
    v             v             v             v
     _________________________________________

              #########     #########
              #       #     #       #
              # BOSS1 #     # BOSS2 #
              #       #     #       #
              #########     #########
                  |             |    
                  v             v    
                   _____________

                     #########
                     #       #
                     # SROOM #
                     #       #
                     #########
                         |
                         v
                     #########
                     #       #
                     # START #
                     #       #
                     #########
```


- Objective:
    + Between players it's a race to find the boss room entrance which marks 
    the end of each round. Finding the boss room requires a successive jumping
    through portals into different environments. Defeating the boss grants the
    win to the first team and also gives them a boss reward. 
    + Alternatively, being the last surviving team can win the round when all
    players die or when a PVP encounter occurs. With the enemy team eliminated
    the next portal jump is to the boss room where the surviving team can still
    play for the boss reward.
    + When the boss is defeated the portal to the Safe Room is opened. All
    players will respawn in the Safe Room between rounds with whatever loot
    they have obtained. The Match win is granted to the first team to win X
    number of rounds or best of X.
 
- Thoughts?
    + Should boss fights be seperate between enemy Teams/Players?
        * Third party / Last hit issues...
        * PVPVE Could be fun though.
    + Winning team can steal an item or money from losing team?
        * I don't want a runnaway effect to happen
    + Boss Item is strong but somewhat balanced
        * Enemy team in next PVP encounter could win and steal?




