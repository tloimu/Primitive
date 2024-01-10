# Plan for Exploration, Survival, Builder Game

Basic drivers for player in order of importance:

 - exploration to the unknown
 - base building
 - resource management
 - surviving the nature (gearing, nourishment, hunting, gathering, farming, animal husbandry)
 - player interaction (coop)
 - crafting
 - educational (linking to actual history of how things were/are made)

Basic technical and gameplay decisions:

 - hosted/dedicated multiplayer
 - procedural world (landscape, sun color, atmosphere, fauna, flora, resources) for replayability and ease of development
 - pressures to encourage player to move to or at least visit new regions
 - no story - has to work without
 - no bosses - has to work without - perhaps special beings with non-mandatory loot (lair/nest)?
 - no experience points or levels to grind
 - if skills/traits/perks are part of the game, their importance is minor
 - meaningful crafting

Avoid:

 - excessive mandatory grind
 - too complex survival mechanisms - focus on exploration with only some survival pressure
 - excessively difficult and tedious inventory management - allow presets and also using items nearby


## Difficulty levels

Make game easier:

 - optional sanity
 - optional infections
 - extra starting skill points (if skills are included)
 
Make game faster:

 - faster skill point accumulation (if skills are included)
 - faster crafting
 
Options:

 - Crafting cost: None - Normal - Realistic
 - Nourishment requirements: None - Normal - Realistic
 - Extraction yields: Nx - Normal - Realistic
 - Resource depletion: None - Normal - Realistic
 - Plant growth speed: Immediate - Normal - Realistic
 - Spoilage: None - Normal - Realistic
 - Crafting speed: Immediate - Normal - Realistic
 - Inventory limits - Personal: None - Normal - Realistic
 - Inventory limits - Storage: None - Normal - Realistic
 - Movement speed: Fast - Normal - Realistic
 - Endurance: Infinite - Normal - Realistic
 - Light: Never really dark - Normal - Realistic


## Development Resources

Background material:

 - How To Make Everything - The Series: https://www.youtube.com/watch?v=Lg7kZpTVoms

Camera angles:

 - https://gamedev.stackexchange.com/questions/55621/perfect-fps-camera-angle
 - http://blog.wolfire.com/2013/04/GDC13-Summary-Animation-Bootcamp-Part-5-6
 - =>
	+ FPS kamera: "...lowering the camera a bit ... between the shoulders and the nose" tai "move the camera to center of the character (horizontally) and to the base of the neck (vertically).. " tai "I have now successfully gotten it looking awesome for both 1st person and 3rd person. Pretty challenging! What I ended up doing: The camera at the base of the neck, the weapons are held exactly 90 degrees in relation to the character, the stock of the weapon is just after the neck ends (but on the level of the right shoulder), the camera follows the right hand, and each of the gun specifies the exact position for the weapon to be in the "Golden Ratio" to which the camera moves while changing weapons. Looks seamless! Perfect! Thanks a lot for the hel"
	+ näytä esim. kädet/työkalut/etc. ruudulla, jotta pelaaja näkee mitä tapahtuu vaikkei se olisi aina realistista
 - https://www.gamedeveloper.com/disciplines/first-vs-third-person-a-matter-of-perspective
 - True FPS Camera: https://www.youtube.com/watch?v=yx_pS5ZXseU

## Inventory

Background material:
 - Best Inventory System: https://www.pcgamer.com/which-game-has-the-best-inventory-system/
 - Basic Inventory pick&drop: https://www.youtube.com/watch?v=t1ECs7CfPNE
 - UE5 Multiplayer Inventory: https://www.youtube.com/watch?v=LMO8MSRmbW8
 - BenUI - UI Widgets in C++: https://www.youtube.com/watch?v=T7v3UnL6PNU
 - C++ UI HUD with Health/Power bars: https://www.youtube.com/watch?v=T7v3UnL6PNU

### Inventory Data Model

InventorySystem: ActorComponent (Class Defaults: ComponentReplicates = true)
 bool AddItem
 bool RemoveItem
 bool Move
 MaxSize: integer
 Content: TArray<FSlotStruct> (Details: Replication = Replicated)

FSlotStruct: Structure
 Id: Name
 Count: integer

Add InventorySystem as component to PlayerCharacter

FItemStruct: Structure
 Name: Text
 Description: Text
 Tumbnail: Texture 2D
 ItemClass: Actor/Class Reference (or Interactable)
 StackSize: integer

ItemDatabase: DataTable	<= fill this with all kinds of items
 FItemStruct

Background:
 - UDataTable c++ quick start: https://dev.epicgames.com/community/snippets/7LX/udatatable-quick-start
 - https://docs.unrealengine.com/5.0/en-US/adding-components-to-an-actor-in-unreal-engine/
 - DragnDrop c++: https://forums.unrealengine.com/t/creating-drag-and-drop-ui-using-c/269049
 
Dropping:
 - Create mesh, put it front of player (camera forward vector, actor location), Add Velocity Change Impulse to it to make it fly out a bit


## Actions

**Interact** (E) picks target non-container items. If the item **is container**, then it opens the container's inventory screen with player and container's inventories. When holding Interact-key, the item's interact menu is opened. Default is to have "Pick"-action. Containers also have "Inventory"-action. If item has custom Interact-menu, that is opened.

**Pick all nearby** (F) picks all items within a cylinder in front of the player. Holding down pick-all-key will open a menu to allow picking all or similar items.

**Hit** (LMB) will extract resources or cause damage.

When crafting, player inventory, nearby open container inventories and nearby items on ground are available. This simplifies inventory management when crafting.

**Build** (Q) opens build UI where target blueprint is chosen and needed/available parts are shown. Holding shift (Shift-Q) will create context specific items only. Blueprints can be organized into presets.

**Shortcuts** (0-9) can be organized into presets and chosen which preset is used. Items in the preset are picked up from nearby if not carried already.

**Free look** (RMB) when clicked, toggles the free look mode. When held down, temporarily keeps the free look mode on until released.

 
## Resources

Manual extraction possible.

Automated extraction possible.

Manual transportation. Automated transport possible to a degree mainly to avoid grind. Not Satisfactory-level industry but some QOLs at least. E.g. water gutters/pipes. Maybe allow taming animals for turning wheels etc. allowing to move goods on a conveyer belt/lifting water up etc.? Windmills? Watermills?


## Building

Allow complex buildings.

Allow planning first. Exposes potential issues with landscape before investing resources.

Allow limited landscape modifications.


## Crafting

Ideas: 
 - "blue prints" can be bought from shops or other players - perhaps found on loots
 - when an item has been crafted enough times, it can be crafted without the "blue print" as well
 - crafting an item more times will make crafting it slightly faster and in better quality next time
 - quality of crafted items is based on "blue print" quality as well as sometimes materials quality
 - many materials come in different qualitites e.g.
	+ refining more pure iron takes multiple crafting rounds
	+ better "refineries" help refining better quality materials
	+ better quality materials can be found (e.g. richer ore, stronger fibers, better hides)
 - items have durability - better quality may increase durability as well
 - items have utility feature (e.g. more efficient resource extraction, more space, faster xyz...)


### Resources

 - Iron ore => iron ignot
 - Copper => copper ignot
 - Plant/Animal Fiber => strings, ropes
 - Animal Hide
 - Twigs (wood) => burn, craft
 - Sticks (wood) ~75cm => burn, craft items
 - Long Sticks/Purlins (wood) ~2.75m => build, break into 3 Sticks
 - Trees => Sticks, Logs, Leaved Branches, Resin
 - Logs => Planks
 - Flint => Flint Stone, Flint&steel, sharp items
 - Obsidian => Obsidian Stone, Very sharp hard items good for wood, animals and plants
 - Granite => Granite Stone, blunt items good for wood
 - Small Stones
 - Large Stones => break into small stones
 - Animals => hide, fibers, strings, furs
 - Mud => Building, Vessels
 - Clay => Building, Vessels
 - Grovel => Concrete
 - Calcite Rock => Lime when heated and mixed later with water?
 - Water (salty and sweet)
 - Salt: Found as such or boil from salty water
 - Sand
 - Silica Sand => Glass with Flux
 - Salt marsh Plants => Heat hard in clay vessel to get Flux
 - Borax => Use as Flux for Glass
 - Dried Leaves
 - Animal Fur
 - Coal
 - Oil
 - Diamonds, Saphires, Ametists

### Craftable Items

Craftable items:
 - Resin Torch (table and wooden stick)
 - Flask for liquids: Hide, String
 - Strings: Fiber
 - Yarn (=String): Animal Fur
 - Needle: Wood/Fish Bone
 - Ropes: Fiber, Strings
 - Logs: Trees
 - Chopped wood: Logs 
 - Iron Ignot: Iron Ore
 - Copper Ignot: Copper Ore
 - Meat: Animals
 - Strong Strings: Animals
 - Hide: Animals
 - Berries, fruits and roots: Plants
 - Healing and poison materials: Plants, Molds?
 - Molds?: Plants, rotten plants and edibles?
 - Woven fabric: Fibers
 - Wood Ash: Burn wood
 - Soda Ash: Heat salt marsh plants in clay vessel
 - Charcoal: Heat wood, some from burning wood
 - Grog: Crushed rock to improve brick quality (prevents cracking when brick dries)
 - Glass: Silica Sand
 - Mortar: Wood Ash, Water - Mix into pellets 5:1, dry them and heat them to calcine
 - Armor/Clothes:
	+ Hide clothing (hat/helmet, braces, gloves, shirt/breast plate)
	+ Fabric clothing
	+ Fur clothing
 - Weapons:
	+ Stone Axe: Small Stone, Sticks, Strings (flint/obsidian/granite)
	+ Stone Knife: Small Stone (flint/obsidian/granite)
	+ Arrow: Stick, Improve damage with stringed flint/obsidian and accuracy with feathers
	+ Bow: Long stick, Strings, Improve damage with better Sticks and Strings (need a Tiller on workbench to make bows)
	+ Pike: Long stick, Improve damage with flint/obsidian stringed, Improve damage with hide and strings
	+ Metal Knife: Iron Ignot, Hide, Strings
	+ Sword: Iron Ignot, Hide, Strings
 - Tools:
	+ Hand Drill: Two Sticks
	+ Bow Drill: Long Stick, Short Stick, String
	+ Chisel: Stone
	+ Basket: Thatch
	+ Backpack: Hide, String, Needle, Scissors/Sharp Knife
	+ Stone Shovel: => Faster Foundation buidling and excavating resources
	+ Metal Shovel: => Faster Foundation buidling and excavating resources
	+ Mortar And Pestle: Large Stone, Small Stone
	+ Brick mold (wooden or clay)
	+ Trowel for spreading Mortar (wooden, metal is faster)
	+ Medium Sized Rectangular vessel to wash brick mold (makes brick making faster and better quality)
	+ Water jar/pot/reservoir (wooden leaks, clay holds water better)
 - Machines:
	+ Adobe Oven:
		* Produce: Baking foods
	+ Brick Kiln: 60 clay bricks (non-burned will do)
		* Produce: 50 bricks and 5 kg Wood Ash in 3 hours
	+ Simple Weaving Rig: 4 Sticks, String, Needle, Yarn
		* Produce: Rough Woven Fabric
	+ Fabric loom
	+ Ore Refinery
	+ Charcoal owen
	+ Owen
	+ Small camp fire
	+ Large camp fire
	+ Glass owen
 - Build:
	+ Bed: Thatch
	+ Adobe (dried mud) Brick
	+ Clay Brick / Broken Clay Brick
	+ Foundation (long sticks, logs, planks, stone/bricks + plaster)
	+ Wall (leaved brances, long sticks, logs, bricks, plaster, fabric, hide)
	+ Ceiling
	+ Mud Brick
	+ Clay Brick
	+ Plaster
	+ Nails (wood, iron)
 - Vehicles:
	+ Canoe: Hide, Long Sticks, Small Sticks, Strings
	+ Raft: Logs, long sticks, hide/fabric for sails
	+ Row boat:
	+ Sail Boat: Plank, Fabric, Rope, String, Nails
	+ Cart: Long Sticks, Strings (add wheels for easier pull)
 
Heating changes:
 - Raw Meat => Cooked Meat (edible)
 - Destroy some poisons
 - Destroy some healing remedies
 - Extract some healing remedies


Wood (https://www.youtube.com/watch?v=FL0p3uxKNd8):
 - Trees =[axe/saw]=> Logs, Long Sticks and Short Sticks
 - Logs =[3x wedge + hammer + axe]=> Split Logs (0.5h)
 - Split Logs =[3x wedge + hammer + axe]=> Long Sticks (0.5h each) 
 - Stave => bow

Stone Chisel cuts longstick in 15 min
Antler Chisel for e.g. removing bark from long stick in 3 minutes or splitting long stick
Sharp rock, splintered from a rock for smoothing, cutting, scraping etc.

Simple WorkBench: long sticks, small sticks
 - log work

Full WorkBench: String, small stone, long sticks, small sticks
 - includes bow tiller
 - includes

Bow making for beginners: https://www.youtube.com/watch?v=8CpOJyDZJvE
Full stone age bow build tutorial: https://www.youtube.com/watch?v=8FlpUJW5C-o


Tiller Tree is faster (and easier) than Floor Tillering


 https://www.youtube.com/watch?v=uqsWex32Ri8

### Animals

Types of animals and behaviours:

 - Gracer: Follow specific plant food source
 - Predator: Hunt Gracer
 - Roaming: Move around locally for food, water and rest
 - Migration: Move seasonally between two or more locations - breeding in one, living in another
 - Territorial: Live and breed within a territory

Organize hierarchially to optimize performance. Assign an animal to an area, which is then activated/deactivated based on player location. On deactive areas, check only infrequently if an animal migrates from area to another if the animal is "migrating" or to check whether animal should change state between "migrating" and "non-migrating".

 - Visibility:
	+ Visual tracer from animal to player => hide actor if no visual path exist when further away
 - Detection:
	+ Check only when nearby player every now and then only part of nearby animals at a time (round-robin)
	+ Facing: Movement direction + occasional +/- 90 deg when stopped
	+ Wind direction for smell
	+ When player is detected, make sound and Hunter-like marks of direction of sound
 - Nearby players (max 100-400 m depending on size):
	+ Has actor
	+ May be hidden or visible
 - Far away:
	+ No actor
	+ Only occasional check for migration

 - State variables:
	+ Migrating: Whether animal is moving from area to another
	+ Detected player
	+ Aggrevated (e.g. after being attacked by player or protecting cubs or nest)
	
 - Behaviour when meeting player that depend on Aggrevation level, distance to player and damage taken
	+ Flee
	+ Observe
	+ Threaten
	+ Attack

 - Spieces traits:
	+ Size category (affects draw range - see "Nearby player")
	+ Mobility on land, water and/or air
	+ Hunting style: Ambush, Patrol
	+ Agressiveness (threaten and later attack)
	+ Flinchy (flees easily)
	+ Avoidance
	+ Curious (may lead to taming)
	+ Predator (hunt other gracers and players for food)
	+ Living area: Temperature, moisture and region
	+ Migration patterns (areas where migrating between and times to migrate)

 - Individual trait modifications:
	+ Agressiveness
	+ Curiousness
	+ Flinchy

Deer, Birds:
 - Two living areas, migrating between winter/summer seasons
 - Foraging within current area

Cats, Canine:
 - One living area
 - Territorial
 - Patrol
 
Crocodile:
 - One living area - shallow water
 - Territorial
 - Ambush


Info-panel of target animal:

 - UI shows info on target animal (represents player character's ability that is better than player's ability)
 - Info like species, gender, age, size, state (unaware/sleeping/eating/drinking/cautious/detected/fleeing/agressive/hunting)


### Process for making Glass

Resources:
 - Silica Sand
 - Flux

Get Flux either from Borax (a whiteish crystaline stone) or by making Soda Ash from Salty Marsh Plants. Soda Ash is made by "burn" the plants in clay vessel on top of fire, mixing the ash with water and filtering it and then boiling the water out leaving white Soda Ash poweder.

1:1:1 Sand + Borax/Soda Ash

Heat in Kiln.

### Making Charcoal

Method #1: Clay lined pit (half of 50cm empty ball of clay) and stack of sticks burning from top down. Dowsed with water once burned down. An hour => wood:charcoal 5:1.

Method #2: Mound of wood in mud cover. Cover the mound with mud and leave top open for smoke to exit. 6 holes at the bottom of the mound for air intake. Start fire from top and let it burn down. Mud can be wet still. WHen fire can be seen from intake holes, seal them then and the top 2.5h later and let it simmer. Next day, collect charcoal. 

Leaves some wood, but more charcoal than Method #1 and bigger chunks (better quality for smelting). But the mound is broken and must be remade for next batch. Yield better than Pit method.

Method #3: Charcoal Mud Kiln. Cylinder 50cm tall, 50cm across with 6 air intake holes at the bottom. Load wood and close the top with mud. Intakes closed with fire is seen thru them. 2.5h later, top is closed. Only the top needs rebuilding between batches. Makes more at once. Even better quality charcoal than Mound method and almost full efficiency (80-90%) in volume.

A kiln-full of charcoal makes 20-100g Iron.


### Process of Making Iron

Method #1: River Sand

Get River Sand and Sluice it in flowing water to get heavier Iron Ore separated from the sand. Sluice twice for better iron concentration.

Make Simple Furnace: Clay, Long Stick, Charcoal, Wood

Preheat furnace with wood and then put charcoal and Iron Ore into it in layers. Push air with e.g. centrifugal blower (part of Simple Furnace). Once burnt (1 hour), collect Slag and separate Iron pellets from Slag in Mortel. 2 kg ore + 2 kg charcoal => 100g iron.


### Making Clay Bricks and Mortar

Need brick mold (wooden, soaked in water). Better mold makes faster and better quality bricks. Using wood ash on mold makes it even faster (does not stick to mold that much).

Mix sand into clay to prevent cracking while drying?

Mold clay into brick shapes. Dry for 5 days (protecte from rain). Turn to keep them drying faster. Heat dry near fire next. Improve yield and quality by topping with Grog (Rock Powder) to prevent cracking. Burn 2-3 hours til all is red hot.

50 bricks into single brick Kiln. Brick Kiln takes 60 clay bricks (non-burned will do) to make - will turn into 60 burned bricks. Or 32 Roof Tiles and takes 1.5 - 2 hours.

Produces Wood Ash enough for 50 Ash Pellets (mix with 1L of water).

Dry the Pellets and heat them to calcine. Heat in Brick Kiln about 2 hours til red hot. Crush pellets into a pot => Cement.

Mix Cemet (1) with Sand/Grovel (3) and Water (1) => Mortar. (Mix with stick)

Use Water to soak Bricks when building. (Spread with Trowel)

Brick Foundation ~ min 3 layers of bricks with mortar.

Roof Tiles need Curvy Roof Tile Mold (wooden or clay) and Small Sticks for frames.

Background info:
 - Primitive Technology: https://www.youtube.com/watch?v=ShvAN9bLwnw

### Adobe Bricks And Material

Adobe is dried mud. Mixed with other materials can make it waterproof or more durable. Mix Straws/Hay/Twigs/Fibers for strength.

Adding Cactus Juice or Asphalt Emulsion => waterproof.

Takes a month to dry. Compressing it hard (Compressed Earth Blocks) allows immediate use.


Background:
 - https://www.thoughtco.com/what-is-adobe-sustainable-energy-efficient-177943


## Building System

Foundations from long sticks, logs, planks, stone/bricks + plaster.
Walls and ceilings from leaved brances, long sticks, logs, bricks, plaster, fabric, hide.

Walls need at least same level foundation/previous floor stucture as it itself is (wood only on wood or bricks etc.).

Doorways and Window Frames use half-bricks every other layer (starting from bottom).

Building Brick Wall while mixing Mortar takes about 40 bricks / day?

Gabble Ends (vinokaton päätykolmiot).

Mud Bricks will slowly dissolve to rain when exposed. Clay Bricks with Mortar do not.

Roof:
	+ 10 Purlins (ruotupuut 2.75m)
	+ Mortar to fix Purlins to walls
	+ 30 deg angle is optimal


Kammi: (https://fi.wikipedia.org/wiki/Kammi)
	+ 40 long sticks
	+ 10 short sticks
	+ 100 moss
	
Lumikammi:
	+ paljon lunta
	+ 30 short stick
	
Beehive
	+ wood planks

Valheim: https://steamcommunity.com/sharedfiles/filedetails/?id=2449156045


## Character building

Ideas:
 - no levels, no total XP
 - skill based
	+ skill related XP
	+ strong negative progression on stacked skill bonuses to avoid super-players
	+ actual usage => related skill XP => related skill bonuses
 - No need to "wear/carry" all tools to use them, enough to have them near by e.g. within 5 meters or so. Maybe even workbench would allow different blueprints/build qualities depending on tools available on or near character?
 - Could new blueprints be found from old huts and by studying structures or finding blueprint books/notes/pictures? And building a "thing" a few times (some random variation) could offer an improved variation of the same?
 - Water Temperature? => affects what grows/lives in the water
 - rumour system: at some points, player gets rumors of resource locations. Player may start with some rumours as an initial hint of "what to do now"

Skills:
 - scavange => improve yield
 - healing => successful treatment, efficient remedy usage
	+ open wounds
	+ infections
	+ psychology? => sanity => max stamina and recovery rate?
 - melee => improved hit/crit chance/damage and defence
 - ranged => improved hit/crit chance
 - study materials and items? => improve crafting skills and resource efficiency?
 - craft => more efficient resource usage, improved item quality, decreased crafting time
	+ earth refinement => better quality materials, more efficient resource usage
	+ use of animal and plant resources => better quality materials, more efficient resource usage
	+ durable materials and structures? => improve durability of crafted items
 - achievements: tell player what sort of things to expect
	+ first resource X found
	+ extracted N of resource X
	+ first animal X encounter
	+ first human encountered
	+ first water vehicle ride
	+ first carriage used
	+ first enclosure/house built (min 4xfoundations + full walls + ceilings + door) = base
	+ first worker aquired to a new base
	+ 5 bases built
 - stats: gives player sense of scale where the player has already been in
	+ coldest/hottest place visited
	+ northest/southes/highest/lowest place visited

## Project

Next:
 - Day-Night cycle
	+ Fix sun light after sun down beaming thru the Earth (fix light intensity at night?)
	+ Add Moon and stars and some directional/ambient light from them too when visible? (https://dev.epicgames.com/community/learning/tutorials/n2RJ/how-to-make-a-night-sky-and-a-moon-in-unreal-engine-5)
	+ Make sun direction depend on latitude and time of year (and longitude?)
 - Add sound to hitting, digging and harvesting (StarterContent has some Audio Cues) and ambience (no birds at night)
	+ https://forums.unrealengine.com/t/how-to-play-a-soundcue-in-c/339969
 - Create Final Packaging (File->Package Project? where?)
 - Refactor Inventory management and make Containers that can hold items
 - Foliage instance data to hold "consumption level/HP" and gained resource types and weights
 - Harvesting resources from trees, rocks, bushes etc.
 - Decide crafting mechanism: Hand craftable, Crafting benches, Saw Mill, Rope Bench etc.
 - Swimming (add Physical Volume with Boyance? add swimming animations and state)
 - Vaulting - to climb trivial places and out of water e.g. onto sea ice
 - Caves?
 - Weather:
	+ Sunny, Partially Sunny, Cloudy, Rain, Snow
	+ Wind direction and stength
	+ Q: How to change Wind for foliage?
	+ Q: How to change color of water (Cloudy vs Sunny)? How to change waves based on Wind?
	+ NxQ: How to make snow on top of everything?
 - Local temperature and humidity variations:
	+ Night/Day cycle
	+ Affect Fog and Rain
	+ Seasonal cycle?
 - Save/Load game

Fix/Improve:
 - Foliage performance, especially add thick grass
 - Different trees and rocks on different biomes
 - "Cliffs" on hills or mountains (e.g. add "steepness" noise map to indicate at which height and how high is a "jump" in final height map to produce steep cliffs)
 - Different kinds of rocks as well for different resources like flint (fire) vs hard rock (tools) vs sand rock (builds)
	+ Some rocks disappear once harvested, others stay harvestable (but slow)
	+ Perlin Noise for rock types?
 - Use fractal noise for base land i.e. N times Perlin noise, each with double frequency and halved amplitude from previous

Lumiraja levelyspiirillä (https://fi.wikipedia.org/wiki/Lumiraja)
 0 - 4500..5200
 30 - 4200..6100
 60 - 600..2500
 80 - 100..400

Puuraja 6.5C (+/- 1.0C) (https://fi.wikipedia.org/wiki/Puuraja)

Sademäärä (https://fi.wikipedia.org/wiki/Kuivuusraja)
 150-250 - Aavikko
 250-


# Project Types

## Project Ark-like

 - Harvesting trees, rocks, bushes => resources, when harvested, set instance variable "harvested" (=>occlusion mask=0f) and timer for resetting the "harvested mask"
 - Food, water, hunger and thirst
 - Building
 - Crafting
 - Swimming
 - Stats, exp and level
 - Blueprint points per level to buy new blueprints
 - NPCs (dinos, animals, ...)
 - Prehistoric Beasts MOD: https://steamcommunity.com/sharedfiles/filedetails/?id=2251183445
 - Prehistoric Structures MOD: https://steamcommunity.com/sharedfiles/filedetails/?id=1853216162

## Project Frontier

You start from your Home Town tasked to 4X the World in the name of the Home Town. 

In multiplayer scenario may start from the same or different Home Towns. Each Faction has their own Home Town.

 - Essence: Explore "the frontier" and "civilize" it
 - Exploration: Harvesting, mining
 - Building: Build shelters, houses, bases in the "frontier" or established town
 - Bringing resources to Home Town
	+ helps it grow
	+ bigger Home Town produce new inventions (blueprints) faster or improve old ones
	+ place new buildings to Home Town (to avoid making AI to do it)?
	+ allow building new Home Towns?
 - Civilizing:
	+ Colonize: Start from home town -> lure people from there to newly explored locations -> new trading posts -> new towns, grow old towns, nation building as the end game?
	+ Wild life and native folk -> Preserve, integrate or conquer?
 - Bringing new town folk avoids excessive grinding as they can start mass producing things (mining, smelting, harvesting...). Allows buying items from towns/trade posts/farms. Exploring and expanding to new areas still needs hands on harvesting and crafting. Respawns work to built areas as well (someone found you and healed you etc.).
 - Conflict with local flora, fauna and natives is mid-game decisions. How to affect how your habitation will co-op with locals? Nation building as end-game where choosing to conquer more to the original home town or start a new nation?
 - Multiplayer options:
	+ coop within the same home town
	+ pvp from different starting home towns (far or near each other in the beginning)


### Multiplayer

Default is PVE + semi-voluntary PVP.

#### Prevent Build Spamming and Promote Casual PVP

Allow for some safe bases, while prevent base spamming.

Each player gets fixed amount of Land Grants - flags that claim a patch of land to the player. Some Safe Land Grants and others Free Land Grants. Safe Grants are PVP-safe, but are very limited. There are more Free Grants, but are not PVP-safe. Building can be done in own lands only.

Allow PVP server settings:
	- no PVP | PVP outside Safe Grants only (default) | PVP outside any Grants only
	- amount of Safe Land Grants (default 2)
	- amount of Free Land Grants (default 8)
	- size of Safe Land Grant (default 100m x 100m)
	- size of Free Land Grant (default 100m x 100m)

Grants can be placed so that they do not intersect or are near (larger range than actual Grant area) with other existing grants of other players. Grants can be moved, but buildings outside player's Grants will soon expire.


# Phases

## Character and Action Basics

	+ Character movement
	+ Character inventory
	+ Pickable/droppable items
	+ Containers
	+ Consumable items

## World Basics

	+ Terrain generation
	+ Resource generation
	
## Refine World

	+ Day/Night cycle (light, temperature)
	+ Weather (wind, rain/snow)
	+ Seasons (temperature, winds, rainfall)
	+ Rivers
	+ Lakes

## Refine Character and Actions

	+ Survival metrics (hunger, thirst, temperature, stamina, encumburance, ...?)
	+ Crafting
	+ Building
	+ Swimming
	+ Digging & Terraforming
	+ Blueprints

## Advanced Character and Actions

	+ Vessels (ships, carts, sleds, ...?)
	+ Crops
	+ Hunting
	+ Taming? (Doggo, Horzees)
	+ Farming

## Advanced World

	+ Wondering Animals
	+ Agressive Animals

## Basic Social World

	+ Home Town
	+ Ruins (buildings, broken carts left by someone, ...?)
	+ NPCs
	+ Dialog with NPCs
	
## Advanced Social World

	+ Background Simulation of Native Tribes
	+ Background Simulation of Home Town
	+ Background Simulation of Nation(s)


## Organizing Code

GameInstance:
 - ItemDb
 - Logged-in player
 - Save/load game

GameState:
 - Date, time
 - Season
 - Weather
 - World

PlayerController (PC):
 - Inventory
 - Hand Crafter
 - Status: Health, Stamina, Hunger?, Thirst?

Character:
 - Input pass to PC
 - Display animations
 - Collisions
 - Targeting from

NpcCharacter:
 - Actor in all clients - Controller is in Server (AI)
 
PlayerState:
 - Name
 - IsOnline


# Sounds

OlivaniSound sets (Audio Odyssey avain HB7MBXP2T3)

Kivenhakkuu
Maan kaivaminen (eri materiaalit)

Puunhakkuu: Medieval Fantasy/Weapons and Armor/Misc/Swoosh'n'Hit.wav
Item equip: Medieval Fantasy/Weapons and Armor/Misc/Item Equip(1/2).wav
Crafting (eri asioille vähän eri äänet)
Ovi auki/kiinni: Hollywood Action/Environment/Doors/Squeaky Wooden Door

(Hollywood Action/Melee Fight/Melee Fight)

Kävely (eri materiaalit - ainakin nurmi, hiekka, kivi, vesi, lumi)
Juoksu?
Uinti
Sukellus
Pintaannousu

Auringonnousu
Illantulo
Keskipäivä
Keskiyö

Puiden lähellä (lintuja)
Veden äärellä

Ukkonen, etäinen, päällä
Tuuli, kova, kevyt, myrsky
Sade, tihku, kova

Veneessä
Kylässä (eri äänet eri kylillä?)

Valikoissa (ehkä Ambient/Exploration (RT 6.0)/Ambient Exploration Main.wav? tai Fantasy vol2 - Long Journey / Casual Exploration)
Taistelussa/vaarassa


# Random Workspace

Tee Item assetit **yhdellä** meshillä:
 - Nuotio: tee yksi mesh blenderissä (lisää "tuhka/hiekkapohja")
 - Soihtu
 - Lisää "SolidFuel" kaikkiin poltettaviin
 - Lisää "Burns"-aika itemeihin ja aseta se sopivaksi kaikkiin poltettaviin
 - Tee joku ruoka, esim. Raw Meat ja Cooked Meat ja pistä kypsennys toimimaan nuotiossa
 - Lisää Fireplace-tallennukseen FuelLeftSeconds-aika
 - Korjaa Ovi ja Oviraamit - ovi on aika nysä! Eikä se käänny oikein.

Tarkenna näppäinten käyttö:
	 - Nuotio: E => on/off, F => Pick, Tab => Inventory?
	 - Box: E => Inventory, F => Pick
	 - Rock: E => Pick, F => Pick all nearby?
	 - Inventory: T@Wearable => Wear/Unwear
 

Lisää Kello, Day, DayOfYear GameSaveen.

 - Muuta SupportedBy-systeemi 1-N => N-N -suhteeksi, jotta esim. katto voi nojata kahteen eri seinään/toisiin kattoihin
 - Ehkä building support-systeemiä pitää vielä miettiä, jotta tuo monitukimalli olisi mahdollista. Nyt tuki selvitetään pelaajan katseen perusteella olevaan tukevan itemin snap-pointiin eikä laitettavan itemin "tukipisteisiin". Pitäis ehkä tehdä "tukipiste"-systeemi, jossa esim. seinässä on myös alhaalla "tukipiste" ja tukipisteet pitäisi osua keskenään. Näin tukipisteiden leikkauksia voisi myös tarkastella jälkikäteen muidenkin vanhojen building itemien suhteen.

Fix:
 - Place Box bugaa, menee maan alle! Droppaus kuitenkin toimii
 - Drop from inventory -> mihin vaan niin se tippuu olis parempi

 - Place-funktiossa ongelma, että esim. foundationin voi laittaa ilmaan roikkumaan, jos asettaessa katsoo ylös. Estä "liian ylös" laittamine. Foundationin pitäisi edes vähän osua maahan, mieluiten kokonaan.
 => REFAKTOROI KOKO Inventory/Crafter/UI-koodi modulaarisemmaksi - tee erikseen "Inventory Slot grid"-widget, "Crafting Slot grid" widget ja kasaile loppu-UI näistä (kts. Satisfactory)
 - Lisää Replace Item-feature, jotta voi vaihtaa osan erilaiseen purkamatta koko kämppää


REDESIGN Inventory, Container ja Crafting UIt
 - "Current Crafting Work" => Crafting-jono (Ark) - Ainaking Crafter UIssä tuo näkyisi


Vähennä muidenkin esineiden pomppuisuutta, ehkä jopa collisionit pois kokonaan?
