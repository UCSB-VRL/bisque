clear mex

mbglfiles = {'astar_search_mex.c', 'bfs_mex.c', 'dfs_mex.c', 'biconnected_components_mex.c', ...
	     'components_mex.c', 'matlab_bgl_sp_mex.c', ...
	     'matlab_bgl_all_sp_mex.c', ...
	     'mst_mex.c', 'clustering_coefficients_mex.c', ...
	     'betweenness_centrality_mex.c', ...
	     'max_flow_mex.c', ...
         'bfs_dfs_vis_mex.c', ...
         'topological_order_mex.c', ...
         'matching_mex.c', ...
         'core_numbers_mex.c', ...
         'dominator_tree_mex.c', ...
         'test_matching_mex.c', ...
         'path_from_pred_mex.c'};
     
c = computer;

large_arrays = 0;
solaris = 0;
mac = 0;

switch (computer)
    case 'PCWIN'
        libname = 'mbgl-pcwin32';
    case 'GLNX86'
        libname = 'mbgl-linux-32';
    case 'MAC'
        libname = 'mbgl-macosx-ppc-32';
    case 'MACI'
        libname = 'mbgl-macosx-intel-32';
    case 'SOL2'
        solaris = 1;
        error('Not currently supported...\n');
    case 'PCWIN64'
        libname = 'mbgl-pcwin64-large';
        large_arrays = 1;
    case 'SOL64'
        solaris = 1;
        error('Not currently supported...\n');
    case 'GLNXA64'
        libname = 'mbgl-linux-64-large';
        large_arrays = 1;
    otherwise
        error('Not currently supported...\n');
end

mexflags = '';

if (large_arrays)
    mexflags = [mexflags ' -largeArrayDims -DMATLAB_BGL_LARGE_ARRAYS '];
end
	     
if (ispc)
    % must change /MD to /ML in mexopts.bat
	%mexflags = '-O -I..\libmbgl\include LINKFLAGS#''$LINKFLAGS -libpath:..\libmbgl\Release'' LINKFLAGSPOST#''$LINKFLAGSPOST libmbgl.lib''';
    mexflags = [mexflags sprintf('-O -I..\\libmbgl\\include LINKFLAGS#''$LINKFLAGS -libpath:..\\libmbgl\\Release'' LINKFLAGSPOST#''$LINKFLAGSPOST lib%s.lib''', libname)];
elseif (mac)
    % mac specific options
elseif (isunix)
    % 
    if (solaris)
    else
        mexflags = [mexflags '-O -I../libmbgl/include -L../libmbgl '];
        %mexflags = [mexflags '-g -D_DEBUG -I../libmbgl/include -L../libmbgl '];
    end
    
    mexflags = [mexflags sprintf('-l%s', libname)];
end;

for file = mbglfiles
     mexstr = ['mex ' mexflags ' ' char(file)];
     fprintf('%s\n', mexstr);
     eval(mexstr);
end;

