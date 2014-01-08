% Mode function for Matlab 6.1 or above
% Syntax: [mode,modal,times] = ms_modal(x);
% where :
%         x is the 1-D input array consisting of JUST numbers in either row or columnar form
%         modal is the output which consists of the numbers appearing in array x. It has each number once only. This is not sorted.
%         times is the array which records how many times each corresponding number in modal occurs
% Example:  
%        x = [1 2 3 4 1 1 2]
%        [mode,modal,times] = ms_modal(x);
% gives:
% mode = 1
% modal =
% 
%      1     2     3     4     0     0
% 
% 
% times =
% 
%      3     2     1     1     0     0
%
%
% Created by Muneem Shahriar (muneem.shahriar@ttu.edu)
% ********************************************************************************************************************
function [mode,modal,times] = mode(x);


len = length(x); % determine the length of the incoming array


% create a matrix equal to the length of the incoming matrix. 
% this matrix will store all the numbers in x, each number ONCE only
% the logic behind this is that, in worst case, all numbers in the incoming matrix may be different
modal = zeros(1,len);  


% times is the corresponding matrix that keeps track how many times the numbers in modal occur
times = zeros(1,len);


    
modal(1) = x(1);   % save the first element of the array in as the first element in modal
times(1) = 0;       % start with 0 for this element. The algorithm below will update it
k = 1;             % this is the position index in the modal and times array when a new element in x occurs

for i = 1:length(x)  % we will traverse the whole matrix
    return_index = ms_checklist(len,modal,x(i));        % return_index determines if the element in x is new or old
    if return_index ~= 0;                               % if the number is an old one...
        times(return_index) = times(return_index) + 1;  % update the count of the old number by 1
    else                                                % a new number has appeared...
        k = k + 1;                                      % k is updated      
        modal(k) = x(i);                                % store our new friend in modal array
        times(k) = times(k) + 1;                        % times(k) was previously 0. Now, we update to 1 (which is logically right!)
    end
end


% determine the index in times that appear most
max_index = 1;                     % consider the first index to be max  
max_times = times(1);              % the number of times the first element in modal appears  

for i = 2:length(times)
    if times(i) > max_times        % if a new element occurs more than old element
        max_index = i;             % store the position where the new element occurs
        max_times = times(i);      % max_times now stores the times this new element occurs
    end
end

% finally, write to the mode
mode = modal(max_index);


% ***********************************************************************************************************************************
% HELPER FUNCTION
% ms_checklist is a small function that returns the index where a number being queried appears in an array
function return_index = ms_checklist(len,modal,x_element)
for j = 1:length(modal) % we will traverse the whole incoming array
    if x_element == modal(j) % if the element is already within the array ...
        return_index = j;          % we return where the element occurs
        break;                     % we break the whole function, and return to main function IMMEDIATELY
    end
    return_index = 0;              % this is a false condition. We all know an element cannot occur at index 0 for an array in Matlab
end

% ***********************************************************************************************************************************
