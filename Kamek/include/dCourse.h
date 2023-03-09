#ifndef __DCOURSE_H
#define __DCOURSE_H

// Proper headers for Course, not the old shit

class dCourse_c {
	public:
		virtual ~dCourse_c();

		void loadFromFile(int courseID);

		struct tilesets_s {
			char name0[32];
			char name1[32];
			char name2[32];
			char name3[32];
		};

		struct settings_s {
			u64 defaultFlags;
			s16 courseFlag;
			s16 timeLimit;
			u8 _[4];
			u8 startEntrance;
			u8 __[3];
		};

		struct bounding_s {
			s32 yBoundNormalTop;
			s32 yBoundNormalBottom;
			s32 yBoundSpecialTop;
			s32 yBoundSpecialBottom;
			u16 entryID;
			u16 lockToBottom;
			u32 unknown;
		};

		struct unk_s {
			u16 unk1;
			u16 unk2;
			u16 unk3;
			u16 unk4;
		};

		struct bg_s {
			u16 entryID;
			u16 xScrollRate;
			u16 yScrollRate;
			u16 yPosOffset;
			u16 xPosOffset;
			u16 fileID1;
			u16 fileID2;
			u16 fileID3;
			u16 unk1;
			u16 scale;
			u32 unk2;
		};

		struct nextGoto_s {
			enum Flag {
				NO_ENTRY = 0x80,
				CONNECTED_PIPE = 8,
				LINK_FORWARD = 4,
				CONNECTED_REVERSE = 1,
			};

			s16 xPos;
			s16 yPos;
			s16 cameraXPos;
			s16 cameraYPos;
			u8 entryID;
			u8 destArea;
			u8 destEntrance;
			u8 type;
			u8 unk1;
			u8 zoneID;
			u8 layerID;
			u8 pathID;
			u16 flags;
			u16 unk2;
		};

		struct sprite_s {
			u16 type;
			s16 xPos;
			s16 yPos;
			u16 eventIDs;
			u32 settings;
			u8 zoneID;
			u8 layerID;
			u16 unused;
		};

		struct load_s {
			u16 type;
			u16 unused;
		};

		struct zone_s {
			s16 xPos;
			s16 yPos;
			s16 xSize;
			s16 ySize;
			u16 modelShading;
			u16 terrainShading;
			u8 entryID;
			u8 boundingID;
			u8 scrollMode;
			u8 zoomMode;
			u8 unk10;
			u8 visibility;
			u8 fgID;
			u8 bgID;
			u8 mpBias;
			u8 unk3;
			u8 music;
			u8 audioModifier;
		};

		struct rect_s {
			s16 xPos;
			s16 yPos;
			s16 xSize;
			s16 ySize;
			u8 entryID;
			u8 unused[3];
		};

		struct cameraFlag_s {
			u32 unk1;
			u32 unk2;
			u32 unk3;
			u8 unk4;
			u8 scrollMode;
			u8 zoomMode;
			u8 unk5;
			u16 unk6;
			u8 eventID;
			u8 unk7;
		};

		struct rail_s {
			enum Flag {
				LOOP = 2,
			};

			u8 entryID;
			u8 unknown;
			u16 startNode;
			u16 nodeCount;
			u16 flags;
		};

		struct railNode_s {
			u16 xPos;
			u16 yPos;
			float speed;
			float accel;
			short unk1;
			short unk2;
		};


		int areaNum;


		union {
			struct {
				tilesets_s *tilesets;
				settings_s *settings;
				bounding_s *bounding;
				unk_s *unk;
				bg_s *topBG;
				bg_s *bottomBG;
				nextGoto_s *nextGoto;
				sprite_s *sprite;
				load_s *load;
				zone_s *zone;
				rect_s *rect;
				cameraFlag_s *cameraFlag;
				rail_s *rail;
				railNode_s *railNode;
			};
			void *blocks[14];
		};

		union {
			struct {
				int tilesetsSize, settingsSize, boundingSize, unkSize;
				int topBGSize, bottomBGSize, nextGotoSize, spriteSize;
				int loadSize, zoneSize, rectSize, cameraFlagSize;
				int railSize, railNodeSize;
			};
			int blockSizes[14];
		};

		union {
			struct {
				int tilesetsCount, settingsCount, boundingCount, unkCount;
				int topBGCount, bottomBGCount, nextGotoCount, spriteCount;
				int loadCount, zoneCount, rectCount, cameraFlagCount;
				int railCount, railNodeCount;
			};
			int blockCounts[14];
		};

		sprite_s *zoneFirstSprite[64];
		int zoneSpriteCount[64];
		int zoneFirstSpriteIdx[64];



		bounding_s *getBoundingByID(u8 id);
		bg_s *getTopBGByID(u16 id);
		bg_s *getBottomBGByID(u16 id);
		nextGoto_s *getNextGotoByID(u8 id);
		zone_s *getZoneByID(u8 id, mRect *output = 0);

		u8 getZoneID(u8 id);
		u8 getBoundingIDForZone(u8 id);
		u8 getScrollForZone(u8 id);
		u8 getZoomForZone(u8 id);
		u8 getUnk10ForZone(u8 id);
		u8 getMusicForZone(u8 id);
		u8 getAudioModifierForZone(u8 id);
		u8 getVisibilityForZone(u8 id);
		u8 getTopBGIDForZone(u8 id);
		u8 getBottomBGIDForZone(u8 id);
		u16 getModelShadingForZone(u8 id);
		u16 getTerrainShadingForZone(u8 id);
		u8 getMPBiasForZone(u8 id);
		u16 getWidthForZone(u8 id);

		rect_s *getRectByID(u8 id, mRect *output = 0);

		u8 getZoneIDContainingPosition(Vec *pos);
		u8 getZoneIDContainingRect(mRect16 *rect);

		bool doesZoneContainPosition(Vec *pos, zone_s *zone);
		bool doesZoneContainRect(mRect16 *rect, zone_s *zone);
};


class dCourseFull_c {
	public:
		dCourse_c courses[4];

		void loadFromFile();

		dCourse_c *get(int id) {
			if (courses[id].zone)
				return &courses[id];
			else
				return 0;
		}

		static dCourseFull_c *instance;

		static void createOnHeap(/*EGG::Heap*/void *heap);
		static void deleteInstance();

		// might not be part of this class, dunno
		static void generateLevelFilename(int world, int level, char *outBuffer);
		static char levelFilename[10];
};
/*
class dGameDisplay_c : dBase_c {         //The dGameDisplay_c class, handles things related to the gameScene layout
    public:
        m2d::EmbedLayout_c layout;
        mEf::es2 effect;
        u32 _330;
        u32 _334;
        u32 _338;
        u32 _33C;
        u32 _340;
        u32 _344;
        u32 _348;
        u32 _34C;
        u32 _350;
        u32 _354;
        u32 _358;
        u32 _35C;
        u32 _360;
        u32 _364;
        u32 _368;
        u32 _36C;
        u32 _370;
        u32 _374;
        u32 _378;
        u32 _37C;
        u32 _380;
        u32 _384;
        u32 _388;
        u32 _38C;
        u32 _390;
        u32 _394;
        u32 _398;
        u32 _39C;
        u32 _3A0;
        u32 _3A4;
        dStateMgr_c state;
        u32 _3CC;
        u32 _3D0;
        u32 _3D4;
        u32 _3D8;
        u32 coins;
        u32 timer;
        u32 _3E4;
        u32 score;
        u32 _3EC;
        u32 _3F0;
        u32 _3F4;
        u32 _3F8;
        u32 _3FC;
        u32 _400;
        u32 _404;
        u32 _408;
        u32 _40C;
        u32 _410;
        u32 _414;
        u32 _418;
        u32 _41C;
        u32 _420;
        u32 _424;
        u32 _428;
        u32 _42C;
        u32 _430;
        u32 mustAtLeast2ForScoreToCount;
        u32 _438;
        u32 _43C;
        u32 _440;
        u32 _444;
        u8 _448;
        u8 _449;
        u8 _44A;
        u8 _44B;
        u32 _44C;
        u8 _450;
        u8 _451;
        u8 _452;
        u8 _453;
        u8 _454;
        u8 _455;
        u8 _456;
        u8 _457;
        u32 _458;
        u32 _45C;
        u32 _460;
        u32 _464;
        u32 _468;
        u32 _46C;
        u32 _470;
        u32 _474;
        u32 _478;
        u32 _47C;
        u32 _480;
        u32 _484;
        u32 _488;
        u32 _48C;
        u32 _490;
        nw4r::lyt::Picture* p_collectOff_00;
        nw4r::lyt::Picture* p_collection_00;
        nw4r::lyt::Picture* p_collectOff_01;
        nw4r::lyt::Picture* p_collection_01;
        nw4r::lyt::Picture* p_collectOff_02;
        nw4r::lyt::Picture* p_collection_02;
        nw4r::lyt::Picture* p_marioIcon_00;
        nw4r::lyt::Picture* p_luijiIcon_00;
        nw4r::lyt::Picture* p_kinoB_00;
        nw4r::lyt::Picture* p_kinoY_00;
        u32 _4BC;
        u32 _4C0;
        u32 _4C4;
        u32 _4C8;
        u32 _4CC;
        u32 _4D0;
        u32 _4D4;
        u32 _4D8;
        nw4r::lyt::TextBox* coinBox;
        nw4r::lyt::TextBox* timerBox;
        nw4r::lyt::TextBox* scoreBox;
        u32 _4E8;
        u32 _4EC;
        u32 _4F0;
        u32 _4F4;
        u8  _4F8;
        void* StarCoin1;
        void* StarCoin2;
        void* StarCoin3;
        u8  _505;
        u8  _506;
        u8  _507;
        u8 blob1[0x51C - 0x508];
        u32 _51C;
        u8 blob2[0x5A0 - 0x508];
        u32 _5A0;
       
        static dGameDisplay_c* instance;
        int onCreate_orig();
        int onExecute_orig();
        int patch_fix();
        int doWaitCheck();
        void doHexCoin();
		
		bool isRed;
		bool hasStarted;
		u16 unused;
		nw4r::lyt::Pane* P_TxtTick_00;
		
		//static dGameDisplay_c* instance;
		void updateSpecialStuff(int toAdd);
		void newUpdateTimer(u32 toSet);
		
		int time;
		int redCoinCounter;
		
		int newOnCreate();

		int newOnExecute();

		int newOnDelete();
		int onDelete_orig();

		//int timerAlarm();
		
		USING_STATES(dGameDisplay_c);
		REF_NINTENDO_STATE(ProcGoalEnd);
};
*/
#endif

