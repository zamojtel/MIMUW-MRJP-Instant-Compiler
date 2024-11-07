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

void print_errors(const Data &data){
  for(auto *error : data.m_errors)
    std::cout<<error->to_string()<<std::endl;
  std::cout<<std::endl;
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
  size_t locals = variables.size();

  Data data;
  data.var_to_slots = create_map(variables);
  
  std::string file_name = std::filesystem::path(path_to_file).filename();
  std::string generated_code = generate_code_jasmin(parse_tree,data);
  std::string start = R"abc(
.class public {}
.super java/lang/Object

.method public <init>()V
    aload_0
    invokenonvirtual java/lang/Object/<init>()V
    return
.end method
  
.method public static main([Ljava/lang/String;)V
.limit stack )abc";

  start = dyna_print(start,file_name);
  start+= std::to_string(data.max_stack_size)+"\n";
  start+=".limit locals "+std::to_string(locals+1)+"\n";
  std::string end = "return\n.end method";
  std::string jasmin_code = start + generated_code +end;

  if(data.get_error_count()>0){
    print_errors(data);
  }
  else{
    std::ofstream myfile;

    std::string path_to_baz_file = path_to_file+".j";
    myfile.open(path_to_baz_file);
    myfile << jasmin_code;
    myfile.close();

    std::string run_java = "java -jar lib/jasmin.jar "+path_to_baz_file;

    system(run_java.data());
  }

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