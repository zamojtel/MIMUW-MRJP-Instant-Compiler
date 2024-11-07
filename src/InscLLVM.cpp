#include "Includes.h"

void usage(void) {
  printf("usage: Call with one of the following argument combinations:\n");
  printf("\t--help\t\tDisplay this help message.\n");
  printf("\t(no arguments)\tParse stdin verbosely.\n");
  printf("\t(files)\t\tParse content of files verbosely.\n");
  printf("\t-s (files)\tSilent mode. Parse content of files silently.\n");
}

template<typename... Args>
std::string dyna_print(std::string_view rt_fmt_str, Args&&... args)
{ 
  return fmt::vformat(rt_fmt_str, fmt::make_format_args(args...));
}

int main(int argc, char ** argv)
{
  FILE *input;
  Program parse_tree;
  int quiet = 1;
  
  if(argc!=2){
    std::cout<<"Wrong number of arguments "<<std::endl;
    return 1; 
  }

  std::string path_to_file = argv[1];
  input = fopen(path_to_file.data(), "r");
  if (!input) {
    std::cout<<"Cant open the file"<<std::endl;
    std::perror("error");
    usage();
    exit(1);
  }

  path_to_file = path_to_file.substr(0,path_to_file.length()-4); // we remove a file extension
  parse_tree = pProgram(input);
  fclose(input);

  std::set<std::string> variables = collect_variables(parse_tree);
  Data data;
  DataLLVM data_llvm;

  data.var_to_slots = create_map(variables);
  data_llvm.m_var_to_slots = data.var_to_slots;

  generate_code_llvm(parse_tree,data_llvm);

  constexpr const char * start_llvm = R"abc(
; ModuleID = 'Instant'
source_filename = "Instant"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

module asm ".globl _ZSt21ios_base_library_initv"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: mustprogress noinline norecurse optnone uwtable
define dso_local noundef i32 @main() #0 {{)abc";
  
  std::string end_llvm= R"abc(
    ret i32 0
}

declare i32 @printf(ptr noundef, ...) #1

attributes #0 = { mustprogress noinline norecurse optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 18.1.3 (1ubuntu1)"}
  )abc";

  std::string llvm_code;
  for(size_t i=0;i<data_llvm.m_lines.size();i++)
    llvm_code+=data_llvm.m_lines[i]+"\n";

  std::string entire_llvm = dyna_print(start_llvm);
  entire_llvm+="\n"+llvm_code;
  entire_llvm+=end_llvm;

  std::ofstream myfile_llvm;
  std::string path_to_baz_file = path_to_file+".ll";
  myfile_llvm.open(path_to_baz_file);
  myfile_llvm << entire_llvm;
  myfile_llvm.close();
  
  std::string run_llvm_runtime = "llvm-as ./lib/runtime.ll -o runtime.bc";


  std::string file_name = std::filesystem::path(path_to_file).filename();
  std::string path_to_result_file = path_to_file+".bc"; 
  std::string run_llvm = "llvm-as "+path_to_baz_file+" -o "+path_to_result_file;

  std::cout<<run_llvm<<std::endl;
  system(run_llvm.data());
  std::string run_llvm_result = "lli "+path_to_result_file;
  std::cout<<run_llvm_result<<std::endl;
  system(run_llvm_result.data());

  if (parse_tree)
  {
    if (!quiet) {
      printf("\n[Abstract Syntax]\n");
      printf("%s\n\n", showProgram(parse_tree));
      printf("[Linearized Tree]\n");
      printf("%s\n\n", printProgram(parse_tree));
    }
    free_Program(parse_tree);
    return 0;
  }

  return 1;
}

