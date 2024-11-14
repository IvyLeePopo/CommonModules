DISTFILES += \
    sconscript

HEADERS += \
    $$PWD/jsoncpp/autolink.h \
    $$PWD/jsoncpp/config.h \
    $$PWD/jsoncpp/features.h \
    $$PWD/jsoncpp/forwards.h \
    $$PWD/jsoncpp/json.h \
    $$PWD/jsoncpp/reader.h \
    $$PWD/jsoncpp/value.h \
    $$PWD/jsoncpp/writer.h \
    $$PWD/jsoncpp/json_batchallocator.h \
    $$PWD/jsoncpp/json_internalarray.inl \
    $$PWD/jsoncpp/json_internalmap.inl \
    $$PWD/jsoncpp/json_valueiterator.inl \

SOURCES += \
    $$PWD/jsoncpp/json_reader.cpp \
    $$PWD/jsoncpp/json_value.cpp \
    $$PWD/jsoncpp/json_writer.cpp \
