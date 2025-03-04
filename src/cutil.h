#ifndef CUTIL_H
#define CUTIL_H

#include <QMutex>
#include <QString>

#include <random>

#include "cubiomes/quadbase.h"
#include "cubiomes/util.h"


inline const char* struct2str(int stype)
{
    switch (stype)
    {
    case Desert_Pyramid:    return "desert_pyramid";
    case Jungle_Temple:     return "jungle_temple";
    case Swamp_Hut:         return "swamp_hut";
    case Igloo:             return "igloo";
    case Village:           return "village";
    case Ocean_Ruin:        return "ocean_ruin";
    case Shipwreck:         return "shipwreck";
    case Monument:          return "monument";
    case Mansion:           return "mansion";
    case Outpost:           return "outpost";
    case Ruined_Portal:     return "ruined_portal";
    case Ruined_Portal_N:   return "ruined_portal (nether)";
    case Treasure:          return "treasure";
    case Mineshaft:         return "mineshaft";
    case Fortress:          return "fortress";
    case Bastion:           return "bastion";
    case End_City:          return "end_city";
    case End_Gateway:       return "end_gateway";
    case Ancient_City:      return "ancient_city";
    }
    return "?";
}

struct StartPiece
{
    int stype;
    int biome;
    int start;
    int giant;
    const char *name;
    int row, col; // UI layout
};

static const StartPiece g_start_pieces[] =
{   // the index is used to encode the start pieces in the condition
    // (and should therefore ideally remain constant across upgrades)
    {Village, plains, 0, -1, "plains_fountain_01", 0, 0},
    {Village, plains, 1, -1, "plains_meeting_point_1", 1, 0},
    {Village, plains, 2, -1, "plains_meeting_point_2", 2, 0},
    {Village, plains, 3, -1, "plains_meeting_point_3", 3, 0},
    {Village, desert, 1, -1, "desert_meeting_point_1", 4, 1},
    {Village, desert, 2, -1, "desert_meeting_point_2", 5, 1},
    {Village, desert, 3, -1, "desert_meeting_point_3", 6, 1},
    {Village, savanna, 1, -1, "savanna_meeting_point_1", 0, 1},
    {Village, savanna, 2, -1, "savanna_meeting_point_2", 1, 1},
    {Village, savanna, 3, -1, "savanna_meeting_point_3", 2, 1},
    {Village, savanna, 4, -1, "savanna_meeting_point_4", 3, 1},
    {Village, taiga, 1, -1, "taiga_meeting_point_1", 7, 0},
    {Village, taiga, 2, -1, "taiga_meeting_point_2", 8, 0},
    {Village, snowy_tundra, 1, -1, "snowy_meeting_point_1", 4, 0},
    {Village, snowy_tundra, 2, -1, "snowy_meeting_point_2", 5, 0},
    {Village, snowy_tundra, 3, -1, "snowy_meeting_point_3", 6, 0},
    {Bastion, -1, 0, -1, "units", 0, 0},
    {Bastion, -1, 1, -1, "hoglin_stable", 1, 0},
    {Bastion, -1, 2, -1, "treasure", 2, 0},
    {Bastion, -1, 3, -1, "bridge", 3, 0},
    {Ruined_Portal, -1, 1, 1, "giant_ruined_portal_1", 0, 1},
    {Ruined_Portal, -1, 2, 1, "giant_ruined_portal_2", 1, 1},
    {Ruined_Portal, -1, 3, 1, "giant_ruined_portal_3", 2, 1},
    {Ruined_Portal, -1, 1, 0, "ruined_portal_1", 0, 0},
    {Ruined_Portal, -1, 2, 0, "ruined_portal_2", 1, 0},
    {Ruined_Portal, -1, 3, 0, "ruined_portal_3", 2, 0},
    {Ruined_Portal, -1, 4, 0, "ruined_portal_4", 3, 0},
    {Ruined_Portal, -1, 5, 0, "ruined_portal_5", 4, 0},
    {Ruined_Portal, -1, 6, 0, "ruined_portal_6", 5, 0},
    {Ruined_Portal, -1, 7, 0, "ruined_portal_7", 6, 0},
    {Ruined_Portal, -1, 8, 0, "ruined_portal_8", 7, 0},
    {Ruined_Portal, -1, 9, 0, "ruined_portal_9", 8, 0},
    {Ruined_Portal, -1, 10, 0, "ruined_portal_10", 9, 0},
    {-1,0,0,0,0,0,0}
};

inline QString getStartPieceName(int stype, const StructureVariant *sv)
{
    QString name;
    for (size_t i = 0; ; i++)
    {
        const StartPiece *sp = g_start_pieces + i;
        if (sp->stype < 0) break;
        if (sp->stype != stype) continue;
        if (sp->biome >= 0 && sp->biome != sv->biome) continue;
        if (sp->giant >= 0 && sp->giant != sv->giant) continue;
        if (sp->start != sv->start) continue;
        name += sp->name;
        break;
    }
    return name;
}


// get a random 64-bit integer
static inline uint64_t getRnd64()
{
    static QMutex mutex;
    static std::random_device rd;
    static std::mt19937_64 mt(rd());
    static uint64_t x = (uint64_t) time(0);
    uint64_t ret = 0;
    mutex.lock();
    if (rd.entropy())
    {
        std::uniform_int_distribution<int64_t> d;
        ret = d(mt);
    }
    else
    {
        const uint64_t c = 0xd6e8feb86659fd93ULL;
        x ^= x >> 32;
        x *= c;
        x ^= x >> 32;
        x *= c;
        x ^= x >> 32;
        ret = x;
    }
    mutex.unlock();
    return ret;
}

enum { S_TEXT, S_NUMERIC, S_RANDOM };
inline int str2seed(const QString &str, uint64_t *out)
{
    if (str.isEmpty())
    {
        *out = getRnd64();
        return S_RANDOM;
    }

    bool ok = false;
    *out = (uint64_t) str.toLongLong(&ok);
    if (ok)
    {
        return S_NUMERIC;
    }

    // String.hashCode();
    int32_t hash = 0;
    const ushort *chars = str.utf16();
    for (int i = 0; chars[i] != 0; i++)
    {
        hash = 31 * hash + chars[i];
    }
    *out = hash;
    return S_TEXT;
}


struct IdCmp
{
    enum
    {
        SORT_ID,
        SORT_LEX,
    };

    IdCmp(int mode, int mc, int dim) : mode(mode),mc(mc),dim(dim) {}

    int mode;
    int mc;
    int dim;
    bool operator() (int id1, int id2)
    {
        if (mode == SORT_ID)
            return id1 < id2;
        int v1 = 1, v2 = 1;
        if (mc >= 0)
        {   // biomes not in this version go to the back
            v1 &= biomeExists(mc, id1);
            v2 &= biomeExists(mc, id2);
        }
        if (dim != DIM_UNDEF)
        {   // biomes in other dimensions go to the back
            v1 &= getDimension(id1) == dim;
            v2 &= getDimension(id2) == dim;
        }
        if (v1 ^ v2)
            return v1;
        const char *s1 = biome2str(mc, id1);
        const char *s2 = biome2str(mc, id2);
        if (!s1 && !s2) return id1 < id2;
        if (!s1) return false; // move non-biomes to back
        if (!s2) return true;
        return strcmp(s1, s2) < 0;
    }

    bool isPrimary(int id)
    {
        if (mode == SORT_ID)
            return true;
        if (mc >= 0 && !biomeExists(mc, id))
            return false;
        if (dim != DIM_UNDEF && getDimension(id) != dim)
            return false;
        return true;
    }
};


#endif // CUTIL_H
