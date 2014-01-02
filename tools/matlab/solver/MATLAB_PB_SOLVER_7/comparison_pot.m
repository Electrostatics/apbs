function varargout = comparison_pot(varargin)
% COMPARISON_POT M-file for comparison_pot.fig
%      COMPARISON_POT, by itself, creates a new COMPARISON_POT or raises the existing
%      singleton*.
%
%      H = COMPARISON_POT returns the handle to a new COMPARISON_POT or the handle to
%      the existing singleton*.
%
%      COMPARISON_POT('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in COMPARISON_POT.M with the given input arguments.
%
%      COMPARISON_POT('Property','Value',...) creates a new COMPARISON_POT or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before comparison_pot_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to comparison_pot_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help comparison_pot

% Last Modified by GUIDE v2.5 11-Dec-2009 11:19:09

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @comparison_pot_OpeningFcn, ...
                   'gui_OutputFcn',  @comparison_pot_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end

% End initialization code - DO NOT EDIT


% --- Executes just before comparison_pot is made visible.
function comparison_pot_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to comparison_pot (see VARARGIN)

% Choose default command line output for comparison_pot
handles.output = hObject;
handles.apbssolvated='';
handles.apbsreference='';
handles.mapbssolvated='';
handles.mapbsreference='';
handles.pathapbssolvated='';
handles.pathapbsreference='';
handles.pathmapbssolvated='';
handles.pathmapbsreference='';
handles.cout='';
% Update handles structure
guidata(hObject, handles);

% UIWAIT makes comparison_pot wait for user response (see UIRESUME)
 %uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = comparison_pot_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;



function edit7_Callback(hObject, eventdata, handles)
% hObject    handle to edit7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit7 as text
%        str2double(get(hObject,'String')) returns contents of edit7 as a double
filespec = {'*.dx', 'Open Format files (*.dx)'};
[handles.apbssolvated, handles.pathapbssolvated] = uigetfile(filespec, 'Select the Required File');

if (ischar(handles.apbssolvated)==0)
    handles.apbssolvated='';
elseif strcmp(handles.apbssolvated, '')==0
    addpath(handles.pathapbssolvated)
    [handles.rmin,handles.dime,handles.h]=gridinf(handles.apbssolvated);
end
set(hObject, 'String',handles.apbssolvated);
guidata(hObject,handles);
button = questdlg('Are you sure this is the correct file? You are not able to change it later!','Confirmation','Yes','No','Yes');
if strcmp(button, 'Yes')==1
set(hObject, 'Enable','off');
else 
    msgbox('please, press "enter" and try again, thanks','Confirmation','none');
    set(hObject, 'String','');
    guidata(hObject,handles);
end

% --- Executes during object creation, after setting all properties.
function edit7_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit12_Callback(hObject, eventdata, handles)
% hObject    handle to edit12 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit12 as text
%        str2double(get(hObject,'String')) returns contents of edit12 as a double
filespec = {'*.dx', 'Open Format files (*.dx)'};
[handles.apbsreference, handles.pathapbsreference] = uigetfile(filespec, 'Select the Required File');
if (ischar(handles.apbsreference)==0)
    handles.apbsreference='';
    elseif strcmp(handles.apbsreference, '')==0
    addpath(handles.pathapbsreference)
    [handles.rmin,handles.dime,handles.h]=gridinf(handles.apbsreference);
end
set(hObject, 'String',handles.apbsreference);

guidata(hObject, handles);
button = questdlg('Are you sure this is the correct file? You are not able to change it later!','Confirmation','Yes','No','Yes');
if strcmp(button, 'Yes')==1
set(hObject, 'Enable','off');
else 
    msgbox('please, press "enter" and try again, thanks','Confirmation','none');
        set(hObject, 'String','');
    guidata(hObject,handles);
end

% --- Executes during object creation, after setting all properties.
function edit12_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit12 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit13_Callback(hObject, eventdata, handles)
% hObject    handle to edit13 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit13 as text
%        str2double(get(hObject,'String')) returns contents of edit13 as a double
filespec = {'*.dx', 'Open Format files (*.dx)'};
[handles.mapbssolvated, handles.pathmapbssolvated] = uigetfile(filespec, 'Select the Required File');
if (ischar(handles.mapbssolvated)==0)
    handles.mapbssolvated='';
      elseif strcmp(handles.mapbssolvated, '')==0
    addpath(handles.pathmapbssolvated)
    [handles.rmin,handles.dime,handles.h]=gridinf(handles.mapbssolvated);
end
set(hObject, 'String',handles.mapbssolvated);

guidata(hObject, handles);
button = questdlg('Are you sure this is the correct file? You are not able to change it later!','Confirmation','Yes','No','Yes');
if strcmp(button, 'Yes')==1
set(hObject, 'Enable','off');
else 
    msgbox('please, press "enter" and try again, thanks','Confirmation','none');
        set(hObject, 'String','');
    guidata(hObject,handles);
end

% --- Executes during object creation, after setting all properties.
function edit13_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit13 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit14_Callback(hObject, eventdata, handles)
% hObject    handle to edit14 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit14 as text
%        str2double(get(hObject,'String')) returns contents of edit14 as a double
filespec = {'*.dx', 'Open Format Files (*.dx)'};
[handles.mapbsreference, handles.pathmapbsreference] = uigetfile(filespec, 'Select the Required File');
if (ischar(handles.mapbsreference)==0)
    handles.mapbsreference='';
      elseif strcmp(handles.mapbsreference, '')==0
    addpath(handles.pathmapbsreference)
    [handles.rmin,handles.dime,handles.h]=gridinf(handles.mapbsreference);
end
set(hObject, 'String',handles.mapbsreference);

guidata(hObject, handles);
button = questdlg('Are you sure this is the correct file? You are not able to change it later!','Confirmation','Yes','No','Yes');
if strcmp(button, 'Yes')==1
set(hObject, 'Enable','off');
else 
    msgbox('please, press "Enter" and try again, thanks','Confirmation','none');
        set(hObject, 'String','');
    guidata(hObject,handles);
end

% --- Executes during object creation, after setting all properties.
function edit14_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit14 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% 
function edit25_Callback(hObject, eventdata, handles)
% hObject    handle to edit25 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit25 as text
%        str2double(get(hObject,'String')) returns contents of edit25 as a double

handles.cout = uigetdir('','please, select Directory to Open');
if (ischar(handles.cout)==0)
    handles.cout='';
end
set(hObject, 'String',handles.cout);
%handles.cout=get(hObject,'String');
guidata(hObject,handles);
button = questdlg('Are you sure this is the the correct Directory? You are not able to change it later!','Confirmation','Yes','No','Yes');
if strcmp(button, 'Yes')==1
set(hObject, 'Enable','off');
else 
    msgbox('please, press "Enter" and try again, thanks','Confirmation','none');
        set(hObject, 'String','');
    guidata(hObject,handles);
end


% --- Executes during object creation, after setting all properties.
function edit25_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit25 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

% --- Executes on button press in pushbutton3.
function pushbutton3_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
close
th=msgbox('This GUI will run our code in your MATLAB command windows. you can close any other matlab windows anytime.','Comparision Calculation','none','modal')
waitfor(th)
clc
run mapbs_comparison

