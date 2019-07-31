cc_plugin(
    name = 'joke',
    srcs = [
        #'comm_db.cc',
        'qa_common_tools.cc',
        #'regex_question_doctor.cc',
        #'nlp_question_doctor.cc',
        #'wqa_busi_handler.cc',
        'wqa_mcd_proc.cc',
        'common_stat.cc',
        'joke.cc',
        'util.cc',
        #'time_utils.cpp',
        'qa_stat.cc',
    ],
    deps = [
        '//proto/nlu:dqa_joke_proto',
        '//proto/nlu:dqa_rootmerge_proto',
        '//proto/nlu:dqa_artifact_proto',
        '//proto/nlu:life_service_query_proto',
        '//longconn:longconn',
        '//nlu/sub_service/weather/spider_patternmatch:pattern_component',
        '//common/text/regex:regex',
        '//common/base/string:string',
        '//common/encoding:encoding',
        #'//thirdparty/mysql:mysqlclient',
        '//common/system/concurrency:concurrency',
        '//common/system/time:time',
        '//common/file:file',
        '//common/file:local_file',
        '//common/file:file_change_notifier',
        '//thirdparty/jsoncpp:jsoncpp',
        "//nlu/public/mcp++:tfc",
        "//nlu/public/mcp++:mcd",
        "//nlu/public/mcp++:dcc",
        "//nlu/public/mcp++:ccd",
    ],
    incs = [
        './',
        './inc/',
        #'../../../../thirdparty/jsoncpp',
        '../../../../common/text/regex',
        '../../../../common/base/string',
        '../../public/mcp++/inc/',
    ],
    defs = [
        'SNACC_DEEP_COPY',
    ],
    warning = 'no'
)

cc_plugin(
    name = 'net_complete',
    srcs = [
        'longconn_complete.cpp'
    ],
    incs = [
        './',
        '../../../../common/base_protocol',
    ],
    defs = [
        'SNACC_DEEP_COPY',
    ]
)

