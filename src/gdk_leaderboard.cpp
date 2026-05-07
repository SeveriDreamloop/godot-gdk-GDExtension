#include "gdk_leaderboard.h"
#include "gdk_helpers.h"
#include <templates/local_vector.hpp>
#include <classes/json.hpp>

using namespace godot;

void GDKXblLeaderboardRow::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_gamertag"), &GDKXblLeaderboardRow::get_gamertag);
    ClassDB::bind_method(D_METHOD("get_modern_gamertag"), &GDKXblLeaderboardRow::get_modern_gamertag);
    ClassDB::bind_method(D_METHOD("get_modern_gamertag_suffix"), &GDKXblLeaderboardRow::get_modern_gamertag_suffix);
    ClassDB::bind_method(D_METHOD("get_unique_modern_gamertag"), &GDKXblLeaderboardRow::get_unique_modern_gamertag);
    ClassDB::bind_method(D_METHOD("get_xbox_user_id"), &GDKXblLeaderboardRow::get_xbox_user_id);
    ClassDB::bind_method(D_METHOD("get_percentile"), &GDKXblLeaderboardRow::get_percentile);
    ClassDB::bind_method(D_METHOD("get_rank"), &GDKXblLeaderboardRow::get_rank);
    ClassDB::bind_method(D_METHOD("get_global_rank"), &GDKXblLeaderboardRow::get_global_rank);
    ClassDB::bind_method(D_METHOD("get_column_values"), &GDKXblLeaderboardRow::get_column_values);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "gamertag"), "", "get_gamertag");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "modern_gamertag"), "", "get_modern_gamertag");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "modern_gamertag_suffix"), "", "get_modern_gamertag_suffix");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "unique_modern_gamertag"), "", "get_unique_modern_gamertag");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "xbox_user_id"), "", "get_xbox_user_id");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "percentile"), "", "get_percentile");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "rank"), "", "get_rank");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "global_rank"), "", "get_global_rank");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "column_values", PROPERTY_HINT_ARRAY_TYPE, "Dictionary"), "", "get_column_values");
}

Ref<GDKXblLeaderboardRow> GDKXblLeaderboardRow::create(const XblLeaderboardRow *row) {
	Ref<GDKXblLeaderboardRow> wrapper;
    if (row != nullptr) {
        wrapper.instantiate();
        wrapper->_gamertag = String(row->gamertag);
        wrapper->_modern_gamertag = String(row->modernGamertag);
        wrapper->_modern_gamertag_suffix = String(row->modernGamertagSuffix);
        wrapper->_unique_modern_gamertag = String(row->uniqueModernGamertag);
        wrapper->_xbox_user_id = row->xboxUserId;
        wrapper->_percentile = row->percentile;
        wrapper->_rank = row->rank;
        wrapper->_global_rank = row->globalRank;

        for (size_t i = 0; i < row->columnValuesCount; i++) {
            const char* v = row->columnValues[i];
            wrapper->_column_values.push_back(JSON::parse_string(v));
        }
    }
    return wrapper;
}

void GDKXblLeaderboardColumn::_bind_methods() {
    BIND_ENUM_CONSTANT(Uint64);
    BIND_ENUM_CONSTANT(Boolean);
    BIND_ENUM_CONSTANT(Double);
    BIND_ENUM_CONSTANT(String);
    BIND_ENUM_CONSTANT(Other);

    ClassDB::bind_method(D_METHOD("get_stat_name"), &GDKXblLeaderboardColumn::get_stat_name);
    ClassDB::bind_method(D_METHOD("get_stat_type"), &GDKXblLeaderboardColumn::get_stat_type);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "stat_name"), "", "get_stat_name");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "stat_type", PROPERTY_HINT_ENUM, "Uint64,Boolean,Double,String,Other", PROPERTY_USAGE_DEFAULT, "GDKXblLeaderboardStatType"), "", "get_stat_type");
}

Ref<GDKXblLeaderboardColumn> GDKXblLeaderboardColumn::create(const XblLeaderboardColumn *result) {
	Ref<GDKXblLeaderboardColumn> wrapper;
    if (result != nullptr) {
        wrapper.instantiate();
        wrapper->_stat_name = godot::String(result->statName);
        wrapper->_stat_type = (GDKXblLeaderboardStatType)result->statType;
    }
    
    return wrapper;
}

void GDKXblLeaderboardQuery::_bind_methods() {
    BIND_ENUM_CONSTANT(None);
    BIND_ENUM_CONSTANT(People);
    BIND_ENUM_CONSTANT(Favorites);

    BIND_ENUM_CONSTANT(UserStatBacked);
    BIND_ENUM_CONSTANT(TitleManagedStatBackedGlobal);
    BIND_ENUM_CONSTANT(TitleManagedStatBackedSocial);

    ClassDB::bind_method(D_METHOD("get_xbox_user_id"), &GDKXblLeaderboardQuery::get_xbox_user_id);
    ClassDB::bind_method(D_METHOD("get_scid"), &GDKXblLeaderboardQuery::get_scid);
    ClassDB::bind_method(D_METHOD("get_leaderboard_name"), &GDKXblLeaderboardQuery::get_leaderboard_name);
    ClassDB::bind_method(D_METHOD("get_stat_name"), &GDKXblLeaderboardQuery::get_stat_name);
    ClassDB::bind_method(D_METHOD("get_social_group_type"), &GDKXblLeaderboardQuery::get_social_group_type);
    ClassDB::bind_method(D_METHOD("get_additional_column_leaderboard_names"), &GDKXblLeaderboardQuery::get_additional_column_leaderboard_names);
    ClassDB::bind_method(D_METHOD("is_ascending"), &GDKXblLeaderboardQuery::is_ascending);
    ClassDB::bind_method(D_METHOD("get_max_items"), &GDKXblLeaderboardQuery::get_max_items);
    ClassDB::bind_method(D_METHOD("get_skip_to_xbox_user_id"), &GDKXblLeaderboardQuery::get_skip_to_xbox_user_id);
    ClassDB::bind_method(D_METHOD("get_skip_to_rank"), &GDKXblLeaderboardQuery::get_skip_to_rank);
    ClassDB::bind_method(D_METHOD("get_continuation_token"), &GDKXblLeaderboardQuery::get_continuation_token);
    ClassDB::bind_method(D_METHOD("get_query_type"), &GDKXblLeaderboardQuery::get_query_type);

    ClassDB::bind_method(D_METHOD("set_xbox_user_id", "xbox_user_id"), &GDKXblLeaderboardQuery::set_xbox_user_id);
    ClassDB::bind_method(D_METHOD("set_scid", "scid"), &GDKXblLeaderboardQuery::set_scid);
    ClassDB::bind_method(D_METHOD("set_leaderboard_name", "leaderboard_name"), &GDKXblLeaderboardQuery::set_leaderboard_name);
    ClassDB::bind_method(D_METHOD("set_stat_name", "stat_name"), &GDKXblLeaderboardQuery::set_stat_name);
    ClassDB::bind_method(D_METHOD("set_social_group_type", "social_group_type"), &GDKXblLeaderboardQuery::set_social_group_type);
    ClassDB::bind_method(D_METHOD("set_additional_column_leaderboard_names", "additional_column_leaderboard_names"), &GDKXblLeaderboardQuery::set_additional_column_leaderboard_names);
    ClassDB::bind_method(D_METHOD("set_is_ascending", "is_ascending"), &GDKXblLeaderboardQuery::set_is_ascending);
    ClassDB::bind_method(D_METHOD("set_max_items", "max_items"), &GDKXblLeaderboardQuery::set_max_items);
    ClassDB::bind_method(D_METHOD("set_skip_to_xbox_user_id", "skip_to_xbox_user_id"), &GDKXblLeaderboardQuery::set_skip_to_xbox_user_id);
    ClassDB::bind_method(D_METHOD("set_skip_to_rank", "skip_to_rank"), &GDKXblLeaderboardQuery::set_skip_to_rank);
    ClassDB::bind_method(D_METHOD("set_continuation_token", "continuation_token"), &GDKXblLeaderboardQuery::set_continuation_token);
    ClassDB::bind_method(D_METHOD("set_query_type"), &GDKXblLeaderboardQuery::set_query_type);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "xbox_user_id"), "set_xbox_user_id", "get_xbox_user_id");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "scid"), "set_scid", "get_scid");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "leaderboard_name"), "set_leaderboard_name", "get_leaderboard_name");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "stat_name"), "set_stat_name", "get_stat_name");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "social_group_type", PROPERTY_HINT_ENUM, "None,People,Favorites", PROPERTY_USAGE_DEFAULT, "GDKXblSocialGroupType"), "set_social_group_type", "get_social_group_type");
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "additional_column_leaderboard_names"), "set_additional_column_leaderboard_names", "get_additional_column_leaderboard_names");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_ascending"), "set_is_ascending", "is_ascending");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_items"), "set_max_items", "get_max_items");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "skip_to_xbox_user_id"), "set_skip_to_xbox_user_id", "get_skip_to_xbox_user_id");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "skip_to_rank"), "set_skip_to_rank", "get_skip_to_rank");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "continuation_token"), "set_continuation_token", "get_continuation_token");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "query_type", PROPERTY_HINT_ENUM, "UserStatBacked,TitleManagedStatBackedGlobal,TitleManagedStatBackedSocial"), "set_query_type", "get_query_type");
}

GDKXblLeaderboardQuery::GDKXblLeaderboardQuery() {
	_scid = GDKHelpers::get_scid();
}

Ref<GDKXblLeaderboardQuery> GDKXblLeaderboardQuery::create(const XblLeaderboardQuery &query) {
	Ref<GDKXblLeaderboardQuery> wrapper;
    wrapper.instantiate();
    wrapper->_xbox_user_id = query.xboxUserId;
    wrapper->_scid = query.scid;
    wrapper->_leaderboard_name = String(query.leaderboardName);
    wrapper->_stat_name = String(query.statName);
    wrapper->_social_group_type = (GDKXblSocialGroupType)query.socialGroup;
    
    PackedStringArray additional_column_leaderboard_names;
    for (size_t i = 0; i < query.additionalColumnleaderboardNamesCount; i++) {
        additional_column_leaderboard_names.push_back(String(query.additionalColumnleaderboardNames[i]));
    }
    wrapper->_additional_column_leaderboard_names = additional_column_leaderboard_names;
    wrapper->_is_ascending = query.order == XblLeaderboardSortOrder::Ascending;
    wrapper->_max_items = query.maxItems;
    wrapper->_skip_to_xbox_user_id = query.skipToXboxUserId;
    wrapper->_skip_to_rank = query.skipResultToRank;
    wrapper->_continuation_token = String(query.continuationToken);
    wrapper->_query_type = (GDKXblLeaderboardQueryType)query.queryType;
    return wrapper;
}

XblLeaderboardQuery GDKXblLeaderboardQuery::build() {
	XblLeaderboardQuery query;
    query.xboxUserId = _xbox_user_id;
    strcpy_s(query.scid, sizeof(query.scid), _scid.utf8().get_data());
    query.leaderboardName = _leaderboard_name.is_empty() ? nullptr : _leaderboard_name.utf8().get_data();
    query.statName = _stat_name.is_empty() ? nullptr : _stat_name.utf8().get_data();
    query.socialGroup = (XblSocialGroupType)_social_group_type;
    query.additionalColumnleaderboardNamesCount = (size_t)_additional_column_leaderboard_names.size();

    LocalVector<const char*> acln;
    acln.reserve(_additional_column_leaderboard_names.size());
    for (size_t i = 0; i < _additional_column_leaderboard_names.size(); i++) {
        acln.push_back(_additional_column_leaderboard_names[i].utf8().get_data());
    }
    query.additionalColumnleaderboardNames = acln.ptr();
    query.order = _is_ascending ? XblLeaderboardSortOrder::Ascending : XblLeaderboardSortOrder::Descending;
    query.maxItems = _max_items;
    query.skipToXboxUserId = _skip_to_xbox_user_id;
    query.skipResultToRank = _skip_to_rank;
    query.continuationToken = _continuation_token.is_empty() ? nullptr : _continuation_token.utf8().get_data();
    query.queryType = (XblLeaderboardQueryType)_query_type;
    return query;
}

void GDKXblLeaderboardResult::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_total_row_count"), &GDKXblLeaderboardResult::get_total_row_count);
    ClassDB::bind_method(D_METHOD("get_columns"), &GDKXblLeaderboardResult::get_columns);
    ClassDB::bind_method(D_METHOD("get_rows"), &GDKXblLeaderboardResult::get_rows);
    ClassDB::bind_method(D_METHOD("has_next"), &GDKXblLeaderboardResult::has_next);
    ClassDB::bind_method(D_METHOD("get_next_async", "user", "max_items"), &GDKXblLeaderboardResult::get_next_async);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "total_row_count"), "", "get_total_row_count");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "columns"), "", "get_columns");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "rows"), "", "get_rows");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "has_next"), "", "get_has_next");
}

Ref<GDKXblLeaderboardResult> GDKXblLeaderboardResult::create(size_t buffer_size) {
    Ref<GDKXblLeaderboardResult> wrapper;
    wrapper.instantiate();
    wrapper->_buffer.resize(buffer_size);
	return wrapper;
}

void GDKXblLeaderboardResult::finalize_result() {
    if (_result == nullptr) {
        return;
    }

    _total_row_count = (int64_t)_result->totalRowCount;

    _columns.clear();
    for (size_t i = 0; i < _result->columnsCount; i++) {
        _columns.push_back(GDKXblLeaderboardColumn::create(&_result->columns[i]));
    }

    _rows.clear();
    for (size_t i = 0; i < _result->rowsCount; i++) {
        _rows.push_back(GDKXblLeaderboardRow::create(&_result->rows[i]));
    }

    _has_next = _result->hasNext;
}

Ref<GDKAsyncBlock> GDKXblLeaderboardResult::get_next_async(Ref<GDKUser> user, int64_t max_items) const {
    ERR_FAIL_COND_V_MSG(max_items < 0, nullptr, "max_items must be >= 0");
    
	Ref<GDKAsyncBlock> asyncBlock = GDKAsyncBlock::create(GDKHelpers::get_async_queue());
    asyncBlock->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);
        Dictionary return_data;
        size_t bufferSize = 0;
        HRESULT hr = XblLeaderboardResultGetNextResultSize(async, &bufferSize);
        return_data["hresult"] = hr;
        if (FAILED(hr)) {
            wrapper->emit(return_data);
            return;
        }

        Ref<GDKXblLeaderboardResult> result = GDKXblLeaderboardResult::create(bufferSize);
        hr = XblLeaderboardResultGetNextResult(async, bufferSize, result->buffer_ptrw(), result->result_ptrw(), nullptr);
        return_data["hresult"] = hr;
        if (SUCCEEDED(hr) && result->has_valid_result()) {
            result->finalize_result();
            return_data["leaderboard_result"] = result;
        }

        wrapper->emit(return_data);
    });

    HRESULT hr = XblLeaderboardResultGetNextAsync(user->get_xbox_live_context(), _result, max_items, asyncBlock->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), nullptr, vformat("XblLeaderboardResultGetNextAsync Error: 0x%08ux", (uint64_t)hr));
    return asyncBlock;
}

void GDKLeaderboard::_bind_methods() {
    GDREGISTER_CLASS(GDKXblLeaderboardRow);
    GDREGISTER_CLASS(GDKXblLeaderboardQuery);
    GDREGISTER_CLASS(GDKXblLeaderboardColumn);
    GDREGISTER_CLASS(GDKXblLeaderboardResult);
    GDREGISTER_CLASS(GDKXblLeaderboardQuery);

    ClassDB::bind_static_method(get_class_static(), D_METHOD("get_leaderboard_async", "user", "query"), &GDKLeaderboard::get_leaderboard_async);
}

Ref<GDKAsyncBlock> GDKLeaderboard::get_leaderboard_async(Ref<GDKUser> user, Ref<GDKXblLeaderboardQuery> query) {
	Ref<GDKAsyncBlock> asyncBlock = GDKAsyncBlock::create(GDKHelpers::get_async_queue());
    asyncBlock->set_callback([](XAsyncBlock* async) {
        GDKAsyncBlock* wrapper = reinterpret_cast<GDKAsyncBlock*>(async->context);

        Dictionary return_data;
        size_t bufferSize = 0;
        HRESULT hr = XblLeaderboardGetLeaderboardResultSize(async, &bufferSize);
        return_data["hresult"] = hr;
        if (FAILED(hr)) {
            wrapper->emit(return_data);
            return;
        }

        Ref<GDKXblLeaderboardResult> result = GDKXblLeaderboardResult::create(bufferSize);
        hr = XblLeaderboardGetLeaderboardResult(async, bufferSize, result->buffer_ptrw(), result->result_ptrw(), nullptr);
        return_data["hresult"] = hr;

        if (SUCCEEDED(hr) && result->has_valid_result()) {
            result->finalize_result();
            return_data["leaderboard_result"] = result;
        }

        wrapper->emit(return_data);
    });

    HRESULT hr = XblLeaderboardGetLeaderboardAsync(user->get_xbox_live_context(), query->build(), asyncBlock->get_block());
    ERR_FAIL_COND_V_MSG(FAILED(hr), nullptr, vformat("XblLeaderboardGetLeaderboardAsync Error: 0x%08ux", (uint64_t)hr));
    return asyncBlock;
}
