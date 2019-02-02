function [varargout]=push_relabel_max_flow(A,u,v,options)
% PUSH_RELABEL_MAX_FLOW Goldberg's push-relabel max flow algorithm
%
% This function calls Boost's implementation of Goldberg's push relabel
% algorithm to compute a maximum flow in a graph.
%
% See the max_flow function for calling information and return parameters.
% This function just calls max_flow(...,struct('algname','push_relabel'));
%
% Example:
%   load('graphs/max_flow_example.mat');
%   push_relabel_max_flow(A)

%
% 7 July 2008
% Initial version
%

if (nargin > 1)
    options.algname = 'push_relabel';
else
    options = struct('algname','push_relabel');
end;

varargout = cell(1,max(nargout,1));

[varargout{:}] = max_flow(A,u,v,options);