// #include <unistd.h>

// static char* img_file = NULL;
// static char* workload_fmt = NULL;

// static int parse_args(int argc, char *argv[]) {
//   const struct option table[] = {
//     {"log"             , required_argument, NULL, 'l'},
//     {"help"            , no_argument      , NULL, 'h'},
//     {"workloadfmt"     , required_argument, NULL, 'w'},
//     {0          , 0                , NULL,  0 },
//   };
//   int o;
//   while ( (o = getopt_long(argc, argv, "-lhw:", table, NULL)) != -1) {
//     switch (o) {
//       case 'l': log_dir = optarg; break;
//       case 'w': workload_fmt = optarg; 
//             Assert(strcmp(workload_fmt, "bin") == 0 || strcmp(workload_fmt, "elf") == 0, 
//                   "Wrong workload format:%s\n", workload_fmt);
//             break;
//       case 1: img_file = optarg; return 0;
//       default:
//         printf("Usage: %s [OPTION...] IMAGE [args]\n\n", argv[0]);
//         printf("\t-b,--batch              run with batch mode\n");
//         printf("\t-l,--log=FILE           output log to FILE\n");
//         printf("\t-d,--diff=REF_SO        run DiffTest with reference REF_SO\n");
//         printf("\t-p,--port=PORT          run DiffTest with port PORT\n");
//         printf("\t-w,--workloadfmt=FMT    workload format: bin or elf\n");
//         printf("\n");
//         exit(0);
//     }
//   }
//   return 0;
// }