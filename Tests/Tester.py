import os
from typing import List

def compare_results(input_files : List[str],output_dir : str,expected_output_dir : str):
    cwd = os.getcwd()

    for output_file_name in input_files:
        path = f'{output_dir}/{output_file_name}.output'
        f1 = open(path, "r")
        output = f1.read()
        output = output

        path = f'{expected_output_dir}/{output_file_name}.output'
        f2 = open(path, "r")
        expected_output = f2.read()
        expected_output = expected_output
        # print(output,end=" ")
        if output == expected_output:
            print(f'Test: {output_file_name} passed !!!')
        else:
            print(f'Test: {output_file_name} not passed!!!')
            raise 1
        f1.close()
        f2.close()

def Test_JVM(input_files : List[str],input_dir : str,output_dir : str,excpected_output_dir : str):
    cwd = os.getcwd()
    jasmin_jar = cwd + '/lib/jasmin.jar'
    executable = cwd + '/insc_jvm'
    
    execute_java = 'java -jar '+jasmin_jar
    
    for input_file_name in input_file_names:
        test = input_dir+'/'+input_file_name+'.ins'
        generated_jasmin = f'{input_dir}/{input_file_name}.j'
        print('Content',generated_jasmin)
        run_java_main = f'java {input_file_name}'

        generate_files_cmd = executable + ' ' +test
        os.system(generate_files_cmd)

        run_java =  execute_java +' '+generated_jasmin
        os.system(run_java)
        output_test = f"{output_dir}/{input_file_name}.output"
        command = run_java_main+' > '+output_test

        os.system(command)
    
    compare_results(input_files,output_dir,excpected_output_dir)

def Test_LLVM(input_files : List[str],input_dir : str,output_dir : str,expected_output_dir : str):
    cwd = os.getcwd()
    executable = cwd + '/insc_llvm'
    for input_file_name in input_file_names:
        generated_llvm = f'{input_dir}/{input_file_name}.ll'
        test = input_dir+'/'+input_file_name+'.ins'
      
        generate_files_cmd = executable + ' ' +test
        processed_llvm_file = f'{input_dir}/{input_file_name}.bc'
        os.system(generate_files_cmd)

        run_llvm_as = f'llvm-as {generated_llvm} -o {processed_llvm_file}'  

        os.system(run_llvm_as)

        output_test = f"{output_dir}/{input_file_name}.output"
        run_lli = f'lli {processed_llvm_file} > {output_test}' 
        os.system(run_lli)

    compare_results(input_files,output_dir,expected_output_dir)
    

if __name__ == '__main__':

    cwd = os.getcwd()
    input_dir = f'{cwd}/Tests/NewTestInputs'
    output_dir = f'{cwd}/Tests/NewTestOutputs'
    expected_output_dir = f'{cwd}/Tests/NewTestExpectedOutputs'

    all_files = os.listdir(input_dir)
    all_files.sort()
    print(all_files)
    input_file_names = []
    for j in range(len(all_files)):
        if all_files[j][-3:]=='ins':
            input_file_names.append(all_files[j][:-4])

 
    os.system(f'mkdir -p {cwd}/Tests/NewTestOutputs')

    print("Printing results for jvm")
    print()
    Test_JVM(input_file_names,input_dir,output_dir,expected_output_dir)
    print("Printing results for llvm")
    print()
    Test_LLVM(input_file_names,input_dir,output_dir,expected_output_dir)
    