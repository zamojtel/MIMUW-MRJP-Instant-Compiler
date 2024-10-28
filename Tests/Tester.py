

import os
from typing import List

# class Tester:

def compare_results(input_files : List[str],output_dir : str,expected_output_dir : str):
    cwd = os.getcwd()
    # output_file_names = os.listdir(f'{cwd}{outputs}')
    # output_file_names.sort()

    for output_file_name in input_files:
        path = f'{output_dir}/{output_file_name}.output'
        f1 = open(path, "r")
        output = f1.read()
        output = output

        path = f'{expected_output_dir}/{output_file_name}.output'
        f2 = open(path, "r")
        expected_output = f2.read()
        expected_output = expected_output
        print(output,end=" ")
        if output == expected_output:
            print(f'Test: {output_file_name} passed !!!')
        else:
            print(f'Test: {output_file_name} not passed!!!')
            raise 1123213
        f1.close()
        f2.close()

def Test_JVM(input_files : List[str],input_dir : str,output_dir : str,excpected_output_dir : str):
    # JVM Code Generation
    cwd = os.getcwd()
    jasmin_jar = cwd + '/lib/jasmin.jar'
    executable = cwd + '/build/TestInstant'

    generated_jasmin = cwd + '/GeneratedFiles/generated_jasmin.j'
    execute_java = 'java -jar '+jasmin_jar
    run_java_main = 'java Main'
    
    for input_file_name in input_file_names:
        test = input_dir+'/'+input_file_name+'.ins'
        print(test)
        print()
        generate_files_cmd = executable + ' < ' +test
        print(generate_files_cmd)
        os.system(generate_files_cmd)

        # jasmin code generation
        run_java =  execute_java +' '+generated_jasmin
        print(run_java)
        os.system(run_java)
        output_test = f"{output_dir}/{input_file_name}.output"
        command = run_java_main+' > '+output_test
        print(command)
        os.system(command)
    
    compare_results(input_files,output_dir,excpected_output_dir)

def Test_LLVM(input_files : List[str],input_dir : str,output_dir : str,excpected_output_dir : str):
    cwd = os.getcwd()
    executable = cwd + '/build/TestInstant'
    generated_llvm = cwd + '/GeneratedFiles/generated_llvm.ll'
    for input_file_name in input_file_names:
        test = input_dir+'/'+input_file_name+'.ins'
        print(test)
        print()
        generate_files_cmd = executable + ' < ' +test
        processed_llvm_file = f'{cwd}/GeneratedFiles/generated_llvm.bc'
        print(generate_files_cmd)
        os.system(generate_files_cmd)

        # llvm code generation
        run_llvm_as = f'llvm-as {generated_llvm} -o {processed_llvm_file}'  
        print(run_llvm_as)
        os.system(run_llvm_as)

        output_test = f"{output_dir}/{input_file_name}.output"
        run_lli = f'lli {processed_llvm_file} > {output_test}' 
        os.system(run_lli)

    compare_results(input_files,output_dir,expected_output_dir)
    

if __name__ == '__main__':
    # input_file_names = os.listdir('/home/bartlomiej-zamojtel/Desktop/ZadaniaMRJP/Instant_Compiler/MIMUW-MRJP-Instant-Compiler/Tests/Inputs')
    # output_file_names = os.listdir('/home/bartlomiej-zamojtel/Desktop/ZadaniaMRJP/Instant_Compiler/MIMUW-MRJP-Instant-Compiler/Tests/ExpectedOutputs')
    input_file_names = os.listdir('/home/bartlomiej-zamojtel/Desktop/ZadaniaMRJP/Instant_Compiler/MIMUW-MRJP-Instant-Compiler/Tests/NewTestInputs')
    # output_file_names = os.listdir('/home/bartlomiej-zamojtel/Desktop/ZadaniaMRJP/Instant_Compiler/MIMUW-MRJP-Instant-Compiler/Tests/NewTestOutputs')
    input_file_names.sort()
    # output_file_names.sort()
    print(input_file_names)
    # print(output_file_names)

    for j in range(len(input_file_names)):
        input_file_names[j] = input_file_names[j][:-4]
    cwd = os.getcwd()
    input_dir = f'{cwd}/Tests/NewTestInputs'
    output_dir = f'{cwd}/Tests/NewTestOutputs'
    expected_output_dir = f'{cwd}/Tests/NewTestExpectedOutputs'
    os.system(f'mkdir {cwd}/Tests/NewTestOutputs')

    # Test_JVM(input_file_names,input_dir,output_dir,expected_output_dir)
    print("Printing test for llvm")
    Test_LLVM(input_file_names,input_dir,output_dir,expected_output_dir)
    




