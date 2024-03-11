function compile()
    pcode('startup.m')
    files = dir;
    matver = version;   
    %combine in 2011b>>>>>>
    %if strcmp(matver, '7.13.0.564 (R2011b)')
    %    cd('SimpoleD');
    %    combine;
    %    cd ..
    %end
    %combine in 2011b<<<<<<
    for i = 1 : length(files)
        if (strcmp(files(i).name, '.')== 0)&&(strcmp(files(i).name, '..')==0)
            if files(i).isdir == 1
                cd(files(i).name)           
                if strcmp(matver, '7.13.0.564 (R2011b)')
                    compileSubDir; % SubSystem: Nesting
                else
                    out = 'Simpole now .mdl save as .slx.';
                    disp(out);
                    compileMdlToSlx;
                    renameMdl;
                end
                cd ..
            elseif files(i).isdir == 0
                files(i).name;
                %1113 pcode
                if strcmp(matver, '7.13.0.564 (R2011b)')
                fileType = files(i).name(end-1:end);
                if fileType =='.m'
                    out =['Simpole now pcode :',files(i).name];
                    disp(out);
                    if (strcmp(files(i).name,'compile.m')==0)
                        pcode(files(i).name);
                        delete(files(i).name);
                    end
                end
                end
                %1113 pcode
            end
        end
    end
    out = 'Simpole compile finish.';
    disp(out);
end
function compileSubDir
    
    files = dir;
    for i = 1 : length(files)
       % files(i).name
        if (strcmp(files(i).name, '.')== 0)&&(strcmp(files(i).name, '..')==0)
            if files(i).isdir == 1
                cd(files(i).name);
                compileSubDir; % SubSystem: Nesting
                cd ..
            elseif files(i).isdir == 0

                fileType = files(i).name(end-1:end);
                if fileType == '.c'
                      if (strcmp(files(i).name,'udpcom.c')==0)
                          out = ['Simpole now mex :',files(i).name];
                          disp(out);
                          mex(files(i).name);
                          %delete(files(i).name);
                      end
                end
                if fileType =='.m'
                    out =['Simpole now pcode :',files(i).name];
                    disp(out);
                    pcode(files(i).name);
                    if (strcmp(files(i).name,'slblocks.m')==0)%&&(strcmp(files(i).name,'combine.m')==0))
                    	delete(files(i).name);
                    end
                end
            end
        end
    end

    %cd ..
end

function compileMdlToSlx
    files = dir;
    for i = 1 : length(files)
       % files(i).name
        if (strcmp(files(i).name, '.')== 0)&&(strcmp(files(i).name, '..')==0)&&(strcmp(files(i).name, 'demo')==0)
            if files(i).isdir == 1
                cd(files(i).name);
                compileMdlToSlx; % SubSystem: Nesting
                cd ..
            elseif files(i).isdir == 0
                fileType1 = files(i).name(end-3:end);                
                if fileType1 == '.mdl'
                    open_system(files(i).name);
                    fileName = files(i).name(1:end - 4);
                    fileNameSlx = [fileName, '.slx'];
                    save_system(fileName,fileNameSlx);
                    close_system(fileNameSlx);
                end
            end
        end
    end
end
function renameMdl
    files = dir;
    for i = 1 : length(files)
       % files(i).name
        if (strcmp(files(i).name, '.')== 0)&&(strcmp(files(i).name, '..')==0)&&(strcmp(files(i).name, 'demo')==0)
            if files(i).isdir == 1
                %fileName = files(i).name
                cd(files(i).name);
                renameMdl; % SubSystem: Nesting
                cd ..
            elseif files(i).isdir == 0
                fileType1 = files(i).name(end - 3:end);                
                if fileType1 == '011b'
                    fileName = files(i).name(1:end - 7);
                    movefile(files(i).name,fileName);
                end
            end
        end
    end
end