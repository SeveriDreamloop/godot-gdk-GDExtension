#pragma once

#include <classes/ref_counted.hpp>
#include <variant/typed_array.hpp>
#include "gdk_asyncblock.h"
#include "gdk_user.h"
#include <xsapi-c/leaderboard_c.h>

namespace godot {
class GDKXblLeaderboardRow: public RefCounted {
    GDCLASS(GDKXblLeaderboardRow, RefCounted);

private:
    String _gamertag;
    String _modern_gamertag;
    String _modern_gamertag_suffix;
    String _unique_modern_gamertag;
    uint64_t _xbox_user_id = 0;
    double _percentile = 0.0;
    uint32_t _rank = 0;
    uint32_t _global_rank = 0;
    TypedArray<Dictionary> _column_values;

protected:
    static void _bind_methods();

public:
    static Ref<GDKXblLeaderboardRow> create(const XblLeaderboardRow* row);

    inline String get_gamertag() const { return _gamertag; }
    inline String get_modern_gamertag() const { return _modern_gamertag; }
    inline String get_modern_gamertag_suffix() const { return _modern_gamertag_suffix; }
    inline String get_unique_modern_gamertag() const { return _unique_modern_gamertag; }
    inline int64_t get_xbox_user_id() const { return (int64_t)_xbox_user_id; }
    inline float get_percentile() const { return (float)_percentile; }
    inline int64_t get_rank() const { return (int64_t)_rank; }
    inline int64_t get_global_rank() const { return (int64_t)_global_rank; }
    inline TypedArray<Dictionary> get_column_values() const { return _column_values; }
};

class GDKXblLeaderboardColumn : public RefCounted {
    GDCLASS(GDKXblLeaderboardColumn, RefCounted);

public:
    enum GDKXblLeaderboardStatType : uint32_t {
        Uint64 = XblLeaderboardStatType::Uint64,
        Boolean = XblLeaderboardStatType::Boolean,
        Double = XblLeaderboardStatType::Double,
        String = XblLeaderboardStatType::String,
        Other = XblLeaderboardStatType::Other
    };

private:
    godot::String _stat_name;
    GDKXblLeaderboardStatType _stat_type;

protected:
    static void _bind_methods();

public:
    static Ref<GDKXblLeaderboardColumn> create(const XblLeaderboardColumn* result);

    inline godot::String get_stat_name() const { return _stat_name; }
    inline GDKXblLeaderboardStatType get_stat_type() const { return _stat_type; }
};

class GDKXblLeaderboardQuery: public RefCounted {
    GDCLASS(GDKXblLeaderboardQuery, RefCounted);
    
public:
    enum GDKXblSocialGroupType : uint32_t {
        None = XblSocialGroupType::None,
        People = XblSocialGroupType::People,
        Favorites = XblSocialGroupType::Favorites
    };

    enum GDKXblLeaderboardQueryType : uint32_t {
        UserStatBacked = XblLeaderboardQueryType::UserStatBacked,
        TitleManagedStatBackedGlobal = XblLeaderboardQueryType::TitleManagedStatBackedGlobal,
        TitleManagedStatBackedSocial = XblLeaderboardQueryType::TitleManagedStatBackedSocial
    };

private:
    uint64_t _xbox_user_id = 0;
    String _scid;
    String _leaderboard_name;
    String _stat_name;
    GDKXblSocialGroupType _social_group_type = GDKXblSocialGroupType::None;
    PackedStringArray _additional_column_leaderboard_names;
    bool _is_ascending = true;
    uint32_t _max_items = 0;
    uint64_t _skip_to_xbox_user_id = 0;
    uint32_t _skip_to_rank = 0;
    String _continuation_token;
    GDKXblLeaderboardQueryType _query_type = GDKXblLeaderboardQueryType::UserStatBacked;

protected:
    static void _bind_methods();

public:
    GDKXblLeaderboardQuery();
    static Ref<GDKXblLeaderboardQuery> create(const XblLeaderboardQuery& query);

    inline int64_t get_xbox_user_id() const { return (int64_t)_xbox_user_id; }
    inline String get_scid() const { return _scid; }
    inline String get_leaderboard_name() const { return _leaderboard_name; }
    inline String get_stat_name() const { return _stat_name; }
    inline GDKXblSocialGroupType get_social_group_type() const { return _social_group_type; }
    inline PackedStringArray get_additional_column_leaderboard_names() const { return _additional_column_leaderboard_names; }
    inline bool is_ascending() const { return _is_ascending; }
    inline int64_t get_max_items() const { return (int64_t)_max_items; }
    inline int64_t get_skip_to_xbox_user_id() const { return (int64_t)_skip_to_xbox_user_id; }
    inline int64_t get_skip_to_rank() const { return (int64_t)_skip_to_rank; }
    inline String get_continuation_token() const { return _continuation_token; }
    inline GDKXblLeaderboardQueryType get_query_type() const { return _query_type; }

    inline void set_xbox_user_id(int64_t xbox_user_id) { _xbox_user_id = (uint64_t)xbox_user_id; }
    inline void set_scid(const String& scid) {_scid = scid;}
    inline void set_leaderboard_name(const String& leaderboard_name) { _leaderboard_name = leaderboard_name; }
    inline void set_stat_name(const String& stat_name) { _stat_name = stat_name; }
    inline void set_social_group_type(GDKXblSocialGroupType social_group_type) { _social_group_type = social_group_type; }
    inline void set_additional_column_leaderboard_names(const PackedStringArray& additional_column_leaderboard_names) { _additional_column_leaderboard_names = additional_column_leaderboard_names; }
    inline void set_is_ascending(bool is_ascending) { _is_ascending = is_ascending; }
    inline void set_max_items(int64_t max_items) { 
        ERR_FAIL_COND_MSG(max_items < 0, "max_results must be >= 0");
        _max_items = (int64_t)max_items;
    }

    void set_skip_to_xbox_user_id(int64_t skip_to_xbox_user_id) { _skip_to_xbox_user_id = (uint64_t)skip_to_xbox_user_id; }
    void set_skip_to_rank(int64_t skip_to_rank) {
        uint32_t max = UINT32_MAX;
        ERR_FAIL_COND_MSG(skip_to_rank < 0 || skip_to_rank > UINT32_MAX, vformat("skip_to_rank must be >= 0 and <= %s", max));
        _skip_to_rank = (uint32_t)skip_to_rank; 
    }

    void set_continuation_token(const String& continuation_token) { _continuation_token = continuation_token; }
    void set_query_type(GDKXblLeaderboardQueryType query_type) { _query_type = query_type; }
    XblLeaderboardQuery build();
};

class GDKXblLeaderboardResult: public RefCounted {
    GDCLASS(GDKXblLeaderboardResult, RefCounted);

private:
    XblLeaderboardResult* _result = nullptr;
    PackedByteArray _buffer;

    int64_t _total_row_count;
    TypedArray<GDKXblLeaderboardColumn> _columns;
    TypedArray<GDKXblLeaderboardRow> _rows;
    bool _has_next;

protected:
    static void _bind_methods();

public:
    static Ref<GDKXblLeaderboardResult> create(size_t buffer_size); 
    void finalize_result();

    XblLeaderboardResult** result_ptrw() { return &_result; }
    uint8_t* buffer_ptrw() { return _buffer.ptrw(); }
    inline bool has_valid_result() const { return _result != nullptr; }

    inline int64_t get_total_row_count() const { return _total_row_count; }
    inline TypedArray<GDKXblLeaderboardColumn> get_columns() const { return _columns; }
    inline TypedArray<GDKXblLeaderboardRow> get_rows() const { return _rows; }
    inline bool has_next() const { return _has_next; }

    Ref<GDKAsyncBlock> get_next_async(Ref<GDKUser> user, int64_t max_items) const;
};

class GDKLeaderboard: public RefCounted {
    GDCLASS(GDKLeaderboard, RefCounted);

protected:
    static void _bind_methods();

public:
    static Ref<GDKAsyncBlock> get_leaderboard_async(Ref<GDKUser> user, Ref<GDKXblLeaderboardQuery> query);
};
}

VARIANT_ENUM_CAST(GDKXblLeaderboardColumn::GDKXblLeaderboardStatType);
VARIANT_ENUM_CAST(GDKXblLeaderboardQuery::GDKXblSocialGroupType);
VARIANT_ENUM_CAST(GDKXblLeaderboardQuery::GDKXblLeaderboardQueryType);