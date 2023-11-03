# Edit all the constants to tune the filter
# Then, run the [make py] command to generate the corresponding filter code
# It will also be saved to filter.txt, which gets read by the C program
# From then on, simply run the [make run] command to filter for that specific kind of seed

################################################################

VERSION = 16
# Minecraft version to run the filter on (1.VERSION)
# Some filters have different behavior/don't work depending on version
# Versions 1.7-1.17 are supported, although with varying levels of funcionality


SEEDS_TO_FIND = 10
# How many seeds to output. If the value is -1, continuously outputs seeds until halted by user


OUTPUT_SEED_INFO = True
# Whether to output relevant structure coordinates/info for each seed. If True, outputs this data into a
# data/seedinfo.txt file. Useful for doing secondary checks on the filter output (e.g. lavachecker, etc)


################################################################


SHIPWRECK = False
# If True, filters for a shipwreck within a certain distance of origin.
#
SHIP_DIST = 9	# Maximum chunks away from origin
SHIP_TYPES = {	# Types of shipwreck. Setting one to False removes it from filtering
	"FRONT": True,	# Front half only
	"BACK": True,	# Back half only
	"FULL": True	# Both halves
}
SHIP_LOOT = {	# Minimum requirements for different types of loot (doesn't work in 1.13)
	"IRON": 0,		# Amount of iron ingots (plus craftable from nuggets)
	"GOLD": 0,		# Amount of gold ingots (plus craftable from nuggets)
	"EMERALD": 0,	# Amount of emeralds
	"DIAMOND": 0,	# Amount of diamonds

	"CARROT": 0,	# Amount of carrots
	"WHEAT": 0,		# Amount of wheat
	"TNT": 0		# Amount of TNT
}
SHIP_QUAD = [	# Quadrants to search in
#	negX	posX
	True,	True,	# negZ
	
	True,	True,	# posZ
]

################################################################

RUINED_PORTAL = False
# If True, filters for an above-ground ruined portal within a certain distance of origin.
# Only available in 1.16+
#
R_DIST = 9			# Maximum distance for ruined portal in chunks
LAVA_PORTAL = False	# If True, filters for a portal that has lava (7/13 portal types)
R_LOOT = {			# Minimum requirements for different types of loot:
	"IRON": 0,			# Amount of ingots craftable from iron nuggets. 
	"GOLD": 0,			# Amount of gold ingots craftable from the chest. Does not include gold generating as part of the portal
	"LIGHT": False,		# Fire charge, flint and steel, or a flint with 1 extra iron (on top of the iron requirement)
	"OBSIDIAN": 0,		# Minimum amount of obsidian in the chest
	"LOOTING": 0		# Minimum level of looting. If 0, does not filter for looting (VERY HEAVY FILTER)
}

################################################################

BURIED_TREASURE = False
# If True, filters for a buried treasure within a certain distance from origin
# Only available in 1.13+
#
# Since BTs generate on a per-chunk basis, you can find multiple within the search area
# The BT_COUNT variable allows you to set the minimum AND maximum amount of BTs for the search area
# Generally in Any%, multiple BTs close together are disastrous, as it makes them hard to locate via mapless,
# so usually the default of [1, 1] is recommended.
#
# Set BT_EXCLUSION_DIST to a higher number than BT_INCLUSION_DIST to create a buffer zone right outside the search area, 
# where no BTs will spawn. This is to prevent there being more BTs right outside the inclusion range, which could
# still mess with runners' mapless despite not being counted in the filter.
BT_INCLUSION_DIST = 5	# Maximum chunks that the minimum BTs must be in
BT_EXCLUSION_DIST = 10	# Minimum distance where maximum BTs must not be exceeded
BT_COUNT = [1, 1]		# [Minimum, maximum], both inclusive
BT_LOOT = {				# Minimum requirements for different types of loot:
	"IRON": 0,				# Amount of iron ingots
	"GOLD": 0,				# Amount of gold ingots
	"TNT": 0,				# Amount of TNT
	"EMERALD": 0,			# Amount of emeralds
	"DIAMOND": 0,			# Amount of diamonds (max. possible is 6)
	"FOOD": 0				# Amount of fish (max. possible is 8)
}
BT_QUAD = [		# Quadrants to search in
#	negX	posX
	True,	True,	# negZ
	
	True,	True,	# posZ
]

################################################################

VILLAGE = False
# If True, filters for a village within a certain distance of origin.
#
V_DIST = 9		# Maximum chunks away from origin
V_BIOMES = {	# Village types to filter for. Setting one to False removes it from filtering
	"PLAINS": True,
	"DESERT": True,
	"SAVANNA": True,
	"TAIGA": True,	# 1.10+
	"TUNDRA": True	# 1.14+
}
V_QUAD = [		# Quadrants to search in
#	negX	posX
	True,	True,	# negZ
	
	True,	True,	# posZ
]

################################################################

DESERT_TEMPLE = False
# If True, filters for a desert temple within a certain distance of origin
#
D_DIST = 9			# Maximum distance in chunks
DT_LOOT = {			# Minimum requirements for different types of loot:
	"IRON": 0,			# Amount of iron ingots
	"GOLD": 0,			# Amount of gold ingots
	"DIAMOND": 0,		# Amount of diamonds
	"EMERALD": 0,		# Amount of emeralds
	"GUNPOWDER": 0,		# Amount of gunpowder
	"GODAPPLE": 0		# Amount of enchanted golden apples (not recommended to set this above 1)
}
DT_QUAD = [		# Quadrants to search in
#	negX	posX
	True,	True,	# negZ
	
	True,	True,	# posZ
]

################################################################

JUNGLE_TEMPLE = False
# If True, filters for a jungle temple within a certain distance of origin
#
JT_DIST = 9		# Maximum distance in chunks
JT_QUAD = [		# Quadrants to search in
#	negX	posX
	True,	True,	# negZ
	
	True,	True,	# posZ
]

################################################################

LAVA_POOL = False
# If True, filters for one or more *potential* lava pools within a certain distance of origin
# No guarantees that this works at all for versions other than 1.16 and 1.17
#
# Lava pools may fail to generate due to terrain, caves, ravines, villages, and other factors, which is why these are only
# *potential* lava pools
#
# Turn up the POOL_COUNT variable to increase the number of pools to find in range. This is useful to increase your
# chances that at least one potential pool will actually generate
#
# If you have OUTPUT_SEED_INFO enabled, you can use the built-in Java lava pool checker using the [make lavachecker] command to
# parse through data/seedinfo.txt and automatically narrow down the output seeds to a list with 100% successful lava pools
L_DIST = 9
POOL_COUNT = 1

################################################################

MAGMA_RAVINE = False
# If True, filters for a wide magma ravine in a deep ocean within a certain distance of origin.
# Not 100% accurate. The ravine will successfully generate ~95% of the time, but sometimes it's not exposed in the water
#
RAV_DIST = 9	# Maximum chunks away from origin
RAV_QUAD = [	# Quadrants to search in
#	negX	posX
	True,	True,	# negZ
	
	True,	True,	# posZ
]

################################################################

BASTION = False
# If True, filters for a bastion within a certain distance of origin
#
B_DIST = 8	# Maximum chunks away from origin
B_TYPES = {	# Allowed bastion types. Change one to False to remove it from filtering
	"BRIDGE": True,
	"TREASURE": True,
	"STABLES": True,
	"HOUSING": True
}
B_QUAD = [	# Quadrants to search in
#	negX	posX
	True,	True,	# negZ
	
	True,	True,	# posZ
]

################################################################

FORTRESS = False
# If True, filters for a fortress within a certain distance of origin
#
F_DIST = 8	# Maximum chunks away from origin
DOUBLE_SPAWNER = False	# If True, filters for a fort with 2 spawners right next to each other
F_QUAD = [	# Quadrants to search in
#	negX	posX
	True,	True,	# negZ
	
	True,	True,	# posZ
]

################################################################

STRONGHOLD = False
# If True, filters for a stronghold within a certain distance of origin (min. possible is 1280).
#
# Changing the FSG_SH variable to True changes it to filter for a stronghold with a good blind from (150, 150)
# in a quadrant that has a fortress (min. possible is 16 but this is very rare)
# If fortress filtering is not enabled, it just does this with the pos-pos quadrant by default
#
FSG_SH = False	# If True, filters for proximity to (150, 150) blind coords -- (1200, 1200) overworld, in the quadrant of the fortress
S_DIST = 1300	# Maximum blocks away from origin OR the FSG blind coords if FSG_SH is enabled
S_OCEAN = False	# If True, filters for a deep ocean stronghold (almost always ocean exposed)

################################################################

END_CAGE = False
# If True, filters for an end cage (or lack thereof) with a certain maximum height
#
MAX_CAGE = 0	# Maximum allowed cage height in blocks. Set to 0 to filter for no end cage

################################################################

ZERO_CYCLE_TOWER = False
# If True, filters for specific zero cycle towers under the configurations below
#
ZERO_DIRECTIONS = {	# Which dragon directions are allowed. Setting one to false removes it from filtering
	"FRONT": True,	"BACK": True
}
ZERO_TOWERS = {		# Which zero cycle towers are allowed. Only considers diagonal node zero cycle
	76: True,	79: True,	82: True,	85: True,	88: True,
	91: True,	94: True,	97: True,	100: True,	103: True
}

################################################################

SPAWN_POINT = False
# If True, filters for a spawn point within a certain distance of origin
#
# Note: if you need a playable seed right off the bat (i.e. need seed number and not worldfile, can't change up the world),
# filtering for spawns is reasonable, but for almost every other scenario you should consider alternatives (e.g. changing the 
# world spawn in a worldfile, etc), as spawnpoint filtering is quite slow
#
# By default, uses an approximate spawn-check function which is about 80% accurate
#
SP_DIST_BLOCKS = 40		# Maximum distance in *blocks*
ACCURATE_SPAWN = False	# If set to True, uses a 100% accurate spawn-check function (VERY SLOW)


################################################################
# END OF THE PARAMETERS
# NOTHING TO EDIT BELOW
################################################################


import sys

filter_code = ""

if not 17 >= VERSION >= 7:
	print("Invalid value for VERSION"); quit()
if SEEDS_TO_FIND < -1:
	print("Invalid value for SEEDS_TO_FIND"); quit()

filter_code += f"{VERSION} {SEEDS_TO_FIND} {int(OUTPUT_SEED_INFO)} "


filter_code += f"{int(BASTION)} "
if BASTION:
	filter_code += f"{B_DIST} "

	types = B_TYPES["BRIDGE"] * 8 + B_TYPES["TREASURE"] * 4 + B_TYPES["STABLES"] * 2 + B_TYPES["HOUSING"]
	if types:
		filter_code += f"{types} "
	else:
		print("At least one bastion type must be enabled!"); quit()

	quadrants = B_QUAD[3] * 8 + B_QUAD[1] * 4 + B_QUAD[2] * 2 + B_QUAD[0]
	if quadrants:
		filter_code += f"{quadrants} "
	else:
		print("At least one bastion quadrant must be enabled!"); quit()


filter_code += f"{int(FORTRESS)} "
if FORTRESS:
	filter_code += f"{F_DIST} {int(DOUBLE_SPAWNER)} "

	quadrants = F_QUAD[3] * 8 + F_QUAD[1] * 4 + F_QUAD[2] * 2 + F_QUAD[0]
	if quadrants:
		filter_code += f"{quadrants} "
	else:
		print("At least one fortress quadrant must be enabled!"); quit()


filter_code += f"{int(STRONGHOLD)} "
if STRONGHOLD:
	filter_code += f"{int(FSG_SH)} {S_DIST} {int(S_OCEAN)} "


filter_code += f"{int(VILLAGE)} "
if VILLAGE:
	filter_code += f"{V_DIST} "

	biome_code = V_BIOMES["PLAINS"] * 16 + V_BIOMES["DESERT"] * 8 + V_BIOMES["SAVANNA"] * 4 + V_BIOMES["TAIGA"] * 2 + V_BIOMES["TUNDRA"]
	if biome_code:
		filter_code += f"{biome_code} "
	else:
		print("At least one village biome must be enabled!"); quit()
	
	quadrants = V_QUAD[3] * 8 + V_QUAD[1] * 4 + V_QUAD[2] * 2 + V_QUAD[0]
	if quadrants:
		filter_code += f"{quadrants} "
	else:
		print("At least one village quadrant must be enabled!"); quit()


filter_code += f"{int(RUINED_PORTAL)} "
if RUINED_PORTAL:
	filter_code += f"{R_DIST} {int(LAVA_PORTAL)} "
	filter_code += f'{R_LOOT["IRON"]} {R_LOOT["GOLD"]} {int(R_LOOT["LIGHT"])} {R_LOOT["OBSIDIAN"]} {R_LOOT["LOOTING"]} '


filter_code += f"{int(LAVA_POOL)} "

if LAVA_POOL:
	filter_code += f"{L_DIST} {POOL_COUNT} "


filter_code += f"{int(BURIED_TREASURE)} "
if BURIED_TREASURE:
	filter_code += f"{BT_INCLUSION_DIST} {BT_EXCLUSION_DIST} {BT_COUNT[0]} {BT_COUNT[1]} "
	filter_code += f'{BT_LOOT["IRON"]} {BT_LOOT["GOLD"]} {BT_LOOT["TNT"]} {BT_LOOT["EMERALD"]} {BT_LOOT["DIAMOND"]} {BT_LOOT["FOOD"]} '

	quadrants = BT_QUAD[3] * 8 + BT_QUAD[1] * 4 + BT_QUAD[2] * 2 + BT_QUAD[0]
	if quadrants:
		filter_code += f"{quadrants} "
	else:
		print("At least one buried treasure quadrant must be enabled!"); quit()


filter_code += f"{int(SPAWN_POINT)} "
if SPAWN_POINT:
	filter_code += f"{SP_DIST_BLOCKS} {int(ACCURATE_SPAWN)} "


filter_code += f"{int(ZERO_CYCLE_TOWER)} "
if ZERO_CYCLE_TOWER:
	front_back = ZERO_DIRECTIONS["FRONT"] * 2 + ZERO_DIRECTIONS["BACK"]
	if front_back:
		filter_code += f"{front_back} "
	else:
		print("At least one variable in ZERO_DIRECTIONS must be enabled!"); quit()
	
	towers = (
	ZERO_TOWERS[76] * 512 + ZERO_TOWERS[79] * 256 + ZERO_TOWERS[82] * 128 + ZERO_TOWERS[85] * 64 + ZERO_TOWERS[88] * 32 +
	ZERO_TOWERS[91] * 16 + ZERO_TOWERS[94] * 8 + ZERO_TOWERS[97] * 4 + ZERO_TOWERS[100] * 2 + ZERO_TOWERS[103])
	if towers:
		filter_code += f"{towers} "
	else:
		print("At least one of the end towers must be enabled!"); quit()


filter_code += f"{int(SHIPWRECK)} "
if SHIPWRECK:
	filter_code += f"{SHIP_DIST} "

	ship_types = SHIP_TYPES["FRONT"] * 4 + SHIP_TYPES["BACK"] * 2 + SHIP_TYPES["FULL"]
	if ship_types:
		filter_code += f"{ship_types} "
	else:
		print("At least one shipwreck type must be enabled!"); quit()
	
	filter_code += f'{SHIP_LOOT["IRON"]} {SHIP_LOOT["GOLD"]} {SHIP_LOOT["EMERALD"]} {SHIP_LOOT["DIAMOND"]} '
	filter_code += f'{SHIP_LOOT["CARROT"]} {SHIP_LOOT["WHEAT"]} {SHIP_LOOT["TNT"]} '

	quadrants = SHIP_QUAD[3] * 8 + SHIP_QUAD[1] * 4 + SHIP_QUAD[2] * 2 + SHIP_QUAD[0]
	if quadrants:
		filter_code += f"{quadrants} "
	else:
		print("At least one shipwreck quadrant must be enabled!"); quit()


filter_code += f"{int(MAGMA_RAVINE)} "
if MAGMA_RAVINE:
	filter_code += f"{RAV_DIST} "

	quadrants = RAV_QUAD[3] * 8 + RAV_QUAD[1] * 4 + RAV_QUAD[2] * 2 + RAV_QUAD[0]
	if quadrants:
		filter_code += f"{quadrants} "
	else:
		print("At least one magma ravine quadrant must be enabled!"); quit()


filter_code += f"{int(JUNGLE_TEMPLE)} "
if JUNGLE_TEMPLE:
	filter_code += f"{JT_DIST} "

	quadrants = JT_QUAD[3] * 8 + JT_QUAD[1] * 4 + JT_QUAD[2] * 2 + JT_QUAD[0]
	if quadrants:
		filter_code += f"{quadrants} "
	else:
		print("At least one jungle temple quadrant must be enabled!"); quit()


filter_code += f"{int(DESERT_TEMPLE)} "
if DESERT_TEMPLE:
	filter_code += f"{D_DIST} "
	filter_code += f'{DT_LOOT["IRON"]} {DT_LOOT["GOLD"]} {DT_LOOT["DIAMOND"]} {DT_LOOT["EMERALD"]} {DT_LOOT["GUNPOWDER"]} {DT_LOOT["GODAPPLE"]} '

	quadrants = DT_QUAD[3] * 8 + DT_QUAD[1] * 4 + DT_QUAD[2] * 2 + DT_QUAD[0]
	if quadrants:
		filter_code += f"{quadrants} "
	else:
		print("At least one desert temple quadrant must be enabled!"); quit()


filter_code += f"{int(END_CAGE)} "
if END_CAGE:
	filter_code += f"{MAX_CAGE} "


print("Generated filter code. This code will be saved to filter.txt as well as data/filterhistory.txt.")

ask_for_name = True
n = ""

if len(sys.argv) != 1 and sys.argv[1] == 'noname':
	ask_for_name = False
	
if ask_for_name:
	n = input("Give this filter a name? (Leave empty for no name) -> ")

with open("data/filter.txt", "w") as filter_file:
	filter_file.write(filter_code)

with open("data/filterhistory.txt", "a") as hist_file:
	if len(n.strip()) != 0:
		hist_file.write(n + "\n")
	hist_file.write(filter_code + "\n\n")


print("\n")
print("FULL FILTER CODE: " + filter_code + "\n")
print("This filter code has been saved to data/filter.txt! Run the command [make run] to start finding seeds.")
