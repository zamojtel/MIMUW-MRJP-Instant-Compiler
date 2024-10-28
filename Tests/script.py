
import os

def run_tests():

    # JVM and LLVM file generation
    # ./TestInstant < /home/bartlomiej-zamojtel/Programming/Zadanie_1_No/Text/assignment.txt

    # JVM generated code 
    # java -jar /home/bartlomiej-zamojtel/Desktop/ZadaniaMRJP/Instant_Compiler/MIMUW-MRJP-Instant-Compiler/lib/jasmin.jar 
    # /home/bartlomiej-zamojtel/Desktop/ZadaniaMRJP/Instant_Compiler/MIMUW-MRJP-Instant-Compiler/build/generated_jasmin.j
    cwd = os.getcwd()
    jasmin_jar = cwd +'/lib/jasmin.jar'
    executable = cwd+'/build/TestInstant'
    generated_jasmin = cwd + '/GeneratedFiles/generated_jasmin.j'
    generated_llvm = cwd + '/GeneratedFiles/generated_llvm.j'
    execute_java = 'java -jar '+jasmin_jar
    run_java_main = 'java Main'
    os.system(f'mkdir {cwd}/Tests/Outputs')
    
    for i in range(1,8):
        print('Test number ',i)
        
        test_folder = cwd + '/Tests/Inputs/' 
        test_file = f'test0{i}.ins'
        test = test_folder+test_file
        print(test)
        print()
        generate_files_cmd = executable + ' < ' +test
        print(generate_files_cmd)
        os.system(generate_files_cmd)

        # jasmin code generation
        run_java =  execute_java +' '+generated_jasmin
        print(run_java)
        os.system(run_java)

        output_test = f"{cwd}/Tests/Outputs/test0{i}.output"
        command = run_java_main+' > '+output_test
        print(command)
        os.system(command)

def run_additional_tests():

    # JVM and LLVM file generation
    # ./TestInstant < /home/bartlomiej-zamojtel/Programming/Zadanie_1_No/Text/assignment.txt

    # JVM generated code 
    # java -jar /home/bartlomiej-zamojtel/Desktop/ZadaniaMRJP/Instant_Compiler/MIMUW-MRJP-Instant-Compiler/lib/jasmin.jar 
    # /home/bartlomiej-zamojtel/Desktop/ZadaniaMRJP/Instant_Compiler/MIMUW-MRJP-Instant-Compiler/build/generated_jasmin.j
    cwd = os.getcwd()
    jasmin_jar = cwd +'/lib/jasmin.jar'
    executable = cwd+'/build/TestInstant'
    generated_jasmin = cwd + '/GeneratedFiles/generated_jasmin.j'
    generated_llvm = cwd + '/GeneratedFiles/generated_llvm.j'
    execute_java = 'java -jar '+jasmin_jar
    run_java_main = 'java Main'
    os.system(f'mkdir {cwd}/Tests/Outputs2')
    for i in range(1,14):
        print('Test number ',i)
        
        test_folder = cwd + '/Tests/StolenInputs/' 
        test_file = f'test0{i}.ins'
        test = test_folder+test_file
        print(test)
        print()
        generate_files_cmd = executable + ' < ' +test
        print(generate_files_cmd)
        os.system(generate_files_cmd)
        
        run_java =  execute_java +' '+generated_jasmin
        print(run_java)
        os.system(run_java)

        if i<10:
            t_file = f'test0{i}'
        else:
            t_file = f'test{i}'
        output_test = f"{cwd}/Tests/Outputs/{t_file}.output"
        command = run_java_main+' > '+output_test
        print(command)
        os.system(command)


def compare_results(outputs : str,expected_outputs : str):
    cwd = os.getcwd()
    for i in range(1,8):
        if i<10:
            path = f'{cwd}{outputs}/test0{i}.output'
        else:
            path = path = f'{cwd}{outputs}/test{i}.output'
        f1 = open(path, "r")
        output = f1.read()
        output = output.strip()
        if i<10:
            path = f'{cwd}{outputs}/test0{i}.output'
        else:
            path = f'{cwd}/{expected_outputs}/test0{i}.output'
        f2 = open(path, "r")
        expected_output = f2.read()
        expected_output = expected_output.strip()
        print(output,end=" ")
        if output == expected_output:
            print('test',i,'passed')
        f1.close()
        f2.close()

def compare_additional_results(outputs : str,expected_outputs : str):
    cwd = os.getcwd()
    for i in range(1,14):
        if i<10:
            path = f'{cwd}{outputs}/test0{i}.output'
        else:
            path = path = f'{cwd}{outputs}/test{i}.output'
        f1 = open(path, "r")
        output = f1.read()
        output = output.strip()
        if i<10:
            path = f'{cwd}{outputs}/test0{i}.output'
        else:
            path = f'{cwd}/{expected_outputs}/test{i}.output'
        f2 = open(path, "r")
        expected_output = f2.read()
        expected_output = expected_output.strip()
        print(output,end=" ")
        if output == expected_output:
            print('test',i,'passed')
        f1.close()
        f2.close()


if __name__ == '__main__':
    # run_tests()
    # compare_results('/Tests/Outputs','/Tests/ExpectedOutputs')
    run_additional_tests()
    compare_additional_results('/Tests/Outputs2','/Tests/StolenExpectedOutputs')
