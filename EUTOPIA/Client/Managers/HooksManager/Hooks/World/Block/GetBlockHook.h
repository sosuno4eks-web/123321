#pragma once
#include <iostream>
#include <string>

#include "../../../../ModuleManager/Modules/Category/Render/OreESP.h"
#include "../../FuncHook.h"
#include "../../../../NotificationManager/NotificationManager.h"

class GetBlockHook : public FuncHook {
   private:
    using func_t = Block*(__thiscall*)(BlockSource*, const BlockPos&);
    static inline func_t oFunc;

    static Block* BlockSourceGetBlockCallback(BlockSource* _this, const BlockPos& blockPos) {
        Block* result = oFunc(_this, blockPos);
        static PortalESP* portalEsp = ModuleManager::getModule<PortalESP>();
        static StorageESP* storageEsp = ModuleManager::getModule<StorageESP>();
        static OreESP* oreEsp = ModuleManager::getModule<OreESP>();

       if (storageEsp->isEnabled()) {
			int blockId = result->blockLegacy->blockId;
           std::string blockName = result->blockLegacy->mtranslateName;

			if(result->blockLegacy && !result->blockLegacy->mtranslateName.empty()) {
				if (storageEsp->chest && (blockId == 54 || blockId == 146)) { // Chest
					storageEsp->blockList[blockPos] = UIColor(255, 165, 0);
				}
				else if (storageEsp->enderChest && blockId == 130) { // Ender Chest
					storageEsp->blockList[blockPos] = UIColor(255, 0, 255);

				} else if(storageEsp->shulkerBox && blockId == 205) {            // Normal Shulker Box
                    storageEsp->blockList[blockPos] = UIColor(255, 255, 255);  // White default
                } else if(storageEsp->shulkerBox && blockId == 218) {          // White
                    storageEsp->blockList[blockPos] = UIColor(255, 255, 255);
                } else if(storageEsp->shulkerBox && blockId == 875) {  // Silver
                    storageEsp->blockList[blockPos] = UIColor(192, 192, 192);
                } else if(storageEsp->shulkerBox && blockId == 874) {  // Gray
                    storageEsp->blockList[blockPos] = UIColor(128, 128, 128);
                } else if(storageEsp->shulkerBox && blockId == 882) {  // Black
                    storageEsp->blockList[blockPos] = UIColor(0, 0, 0);
                } else if(storageEsp->shulkerBox && blockId == 879) {  // Brown
                    storageEsp->blockList[blockPos] = UIColor(139, 69, 19);
                } else if(storageEsp->shulkerBox && blockId == 881) {  // Red
                    storageEsp->blockList[blockPos] = UIColor(255, 0, 0);
                } else if(storageEsp->shulkerBox && blockId == 868) {  // Orange
                    storageEsp->blockList[blockPos] = UIColor(255, 165, 0);
                } else if(storageEsp->shulkerBox && blockId == 871) {  // Yellow
                    storageEsp->blockList[blockPos] = UIColor(255, 255, 0);
                } else if(storageEsp->shulkerBox && blockId == 872) {  // Lime
                    storageEsp->blockList[blockPos] = UIColor(50, 205, 50);
                } else if(storageEsp->shulkerBox && blockId == 880) {  // Dark Green
                    storageEsp->blockList[blockPos] = UIColor(0, 100, 0);
                } else if(storageEsp->shulkerBox && blockId == 876) {  // Cyan
                    storageEsp->blockList[blockPos] = UIColor(0, 255, 255);
                } else if(storageEsp->shulkerBox && blockId == 870) {  // Light Blue
                    storageEsp->blockList[blockPos] = UIColor(173, 216, 230);
                } else if(storageEsp->shulkerBox && blockId == 878) {  // Blue
                    storageEsp->blockList[blockPos] = UIColor(0, 0, 255);
                } else if(storageEsp->shulkerBox && blockId == 877) {  // Purple
                    storageEsp->blockList[blockPos] = UIColor(128, 0, 128);
                } else if(storageEsp->shulkerBox && blockId == 869) {  // Magenta
                    storageEsp->blockList[blockPos] = UIColor(255, 0, 255);
                } else if(storageEsp->shulkerBox && blockId == 873) {  // Pink
                    storageEsp->blockList[blockPos] = UIColor(255, 182, 193);
                }
				else if (storageEsp->barrel && blockId == 458) { // Barrel
					storageEsp->blockList[blockPos] = UIColor(155, 75, 0);
				}
				else if (storageEsp->hopper && blockId == 154) { // Hopper
					storageEsp->blockList[blockPos] = UIColor(128, 128, 128);
				}
				else if (storageEsp->furnace && (blockId == 61 || blockId == 451 || blockId == 453)) { // Furnace
					storageEsp->blockList[blockPos] = UIColor(70, 70, 70);
				}
				else if (storageEsp->spawner && blockId == 52) { // Spawner
					storageEsp->blockList[blockPos] = UIColor(0, 0, 0);
				}
				else if (storageEsp->bookshelf && blockId == 47) { // Bookshelf
					storageEsp->blockList[blockPos] = UIColor(139, 69, 19);
				}
				else if (storageEsp->enchantingTable && blockId == 116) { // Enchanting Table
					storageEsp->blockList[blockPos] = UIColor(0, 0, 255);

				} else if(storageEsp->decoratedPot && blockId == 806) {  // Decorated Pot
					storageEsp->blockList[blockPos] = UIColor(255, 0, 0);
				}
				else if (storageEsp->web && blockName.find("web") != std::string::npos) { // Cobweb
					storageEsp->blockList[blockPos] = UIColor(128, 128, 128);
				}
				else if (storageEsp->dropper && blockName.find("dropper") != std::string::npos) { // Dropper
					storageEsp->blockList[blockPos] = UIColor(192, 192, 192);
				}
				else if (storageEsp->dispenser && blockId == 23) { // Dispenser
					storageEsp->blockList[blockPos] = UIColor(169, 169, 169);
				}
				else if (storageEsp->anvil && blockId == 145) { // Anvil
					storageEsp->blockList[blockPos] = UIColor(105, 105, 105);
				}
			}
		}

        if(oreEsp && oreEsp->isEnabled()) {
            int blockId = result->blockLegacy->blockId;
            std::string blockName = result->blockLegacy->blockName;

            if(result->blockLegacy && !blockName.empty()) {
                if(oreEsp->diamondOre && (blockId == 56)) {  // Diamond Ore
                    oreEsp->blockList[blockPos] = UIColor(0, 255, 255);
                } else if(oreEsp->goldOre && (blockId == 14)) {  // Gold Ore
                    oreEsp->blockList[blockPos] = UIColor(255, 215, 0);
                } else if(oreEsp->ironOre && (blockId == 15)) {  // Iron Ore
                    oreEsp->blockList[blockPos] = UIColor(184, 134, 11);
                } else if(oreEsp->coalOre && (blockId == 16)) {  // Coal Ore
                    oreEsp->blockList[blockPos] = UIColor(54, 54, 54);
                } else if(oreEsp->redstoneOre &&
                          (blockId == 73 || blockId == 74)) {  // Redstone Ore (lit/unlit)
                    oreEsp->blockList[blockPos] = UIColor(255, 0, 0);
                } else if(oreEsp->lapisOre && (blockId == 21)) {  // Lapis Lazuli Ore
                    oreEsp->blockList[blockPos] = UIColor(0, 0, 255);
                } else if(oreEsp->emeraldOre && (blockId == 129)) {  // Emerald Ore
                    oreEsp->blockList[blockPos] = UIColor(0, 255, 0);
                } else if(oreEsp->quartzOre && (blockId == 153)) {  // Nether Quartz Ore
                    oreEsp->blockList[blockPos] = UIColor(255, 255, 255);
                }
            }
        }

        if(portalEsp && portalEsp->isEnabled()) {
            if(result->blockLegacy && !result->blockLegacy->blockName.empty()) {
                if(result->blockLegacy->blockName.find("portal") != std::string::npos) {  // Portal
                    portalEsp->blockList[blockPos] = UIColor(200, 0, 255);
                }
            }
        }
        	static NewChunks* newChunkMod = ModuleManager::getModule<NewChunks>();
		if (newChunkMod->isEnabled()) {
			static uint16_t checkingBlockIds[3] = { 9, 10, 11 };

			static std::vector<std::string> suspiciousSubstrings = {
				"netherite", "crying", "ancient", "trapped", "beacon", "amethyst",
				"shulker", "enchanting", "jukebox", "respawn_anchor", "lodestone",
				"deepslate", "blackstone", "glowing", "unknown", "stonecutter",
				"redstone_torch", "redstone_repeater", "button", "lever", "magma",
				"obsidian", "wool", "bar", "rod", "wall", "rtz_", "_qua", "concrete",
				"kelp", "soil", "pot", "powder", "ender", "portal", "smithing",
				"fletch", "piston", "dropper", "observer", "barrel", "beehive",
				"suspicious", "slightly", "very", "candle", "lantern", "pickle",
				"grindstone", "shroom", "warped", "nylium", "basalt", "weeping",
				"twisting", "blue_ice", "sponge", "reinforced", "sculk", "mangrove",
				"raw", "glazed", "honeycomb", "copper", "dried", "smooth", "brick",
				"scaffold", "gates", "lectern", "stained"
			};

			uint16_t currentBlockId = result->blockLegacy->blockId;
            std::string currentBlockName = result->blockLegacy->mtranslateName;

			bool isSuspiciousId = false;
			for (auto& id : checkingBlockIds) {
				if (currentBlockId == id) {
					isSuspiciousId = true;
					break;
				}
			}

			if (!newChunkMod->detectFlowing) {
				if (currentBlockId == 10 || currentBlockId == 11)
					isSuspiciousId = false;
			}


			std::string lowerBlockName = currentBlockName;
			std::transform(lowerBlockName.begin(), lowerBlockName.end(), lowerBlockName.begin(), ::tolower);

			bool isSuspiciousName = false;
			for (const auto& suspicious : suspiciousSubstrings) {
				std::string lowerSuspicious = suspicious;
				std::transform(lowerSuspicious.begin(), lowerSuspicious.end(), lowerSuspicious.begin(), ::tolower);

				if (lowerBlockName.find(lowerSuspicious) != std::string::npos) {
					isSuspiciousName = true;
					break;
				}
			}

			if (isSuspiciousId || isSuspiciousName) {
				if (oFunc(_this, BlockPos(blockPos.x, blockPos.y - 1, blockPos.z))->blockLegacy->blockId == 0) {
					ChunkPos chunkPos = ChunkPos(blockPos);
					uint64_t chunkHash = std::hash<int>()(chunkPos.x) ^ std::hash<int>()(chunkPos.z);

					if (!newChunkMod->chunkMap[chunkHash].has_value()) {
						newChunkMod->chunkMap[chunkHash] = chunkPos;
						NotificationManager::addNotification("Suspicious block in new chunk detected!", 3.f);
					}
				}
			}
		}
        return result;
    }

   public:
    GetBlockHook() {
        OriginFunc = (void*)&oFunc;
        func = (void*)&BlockSourceGetBlockCallback;
    }
};
