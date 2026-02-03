#pragma once 

#define GDKOBJECT(m_wrapper, m_class, m_target)                 \
class m_wrapper : public m_target {                             \
    GDCLASS(m_wrapper, m_target)                                \
private:                                                        \
    m_class _data = nullptr;                                   \
protected:                                                      \
    static void _bind_methods() {}                              \
public:                                                         \
    static Ref<m_wrapper> create(m_class data) {               \
        Ref<m_wrapper> data_wrapper;                            \
        if (data) {                                             \
            data_wrapper.instantiate();                         \
            data_wrapper->_data = data;                         \
        }                                                       \
        return data_wrapper;                                    \
    }                                                           \
inline m_class get_data() const { return _data; }              \
};