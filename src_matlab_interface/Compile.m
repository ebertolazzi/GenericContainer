clc;

NAMES = { 'test_gc', 'print_recursive' };

disp('---------------------------------------------------------');
SRCS = [...
  '../src/GenericContainer.cc ', ...
  '../src/GenericContainerSupport.cc ', ...
  '../src/GenericContainerTables.cc ', ...
  'GenericContainerInterface_matlab.cc' ...
];

for k=1:2
  N=NAMES{k};
  fprintf(1,'Compiling: %s\n',N);

  CMD = ['mex -I../src -I../include -I../include/GenericContainer -output ',N,' -largeArrayDims mex_',N,'.cc ', SRCS];
  if isunix
    if ismac
      CMD = [CMD, ' -lstdc++ CXXFLAGS="\$CXXFLAGS -Wall -O2 -g"'];
    else
      CMD = [CMD, ' -lstdc++ CXXFLAGS="\$CXXFLAGS -Wall -O2 -g"'];
    end
  elseif ispc
  end
  disp(CMD);
  eval(CMD);
end
disp('----------------------- DONE ----------------------------');
