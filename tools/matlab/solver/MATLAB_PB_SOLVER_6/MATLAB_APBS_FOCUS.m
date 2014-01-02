function varargout = MATLAB_APBS_FOCUS(varargin)
% MATLAB_APBS_FOCUS M-file for MATLAB_APBS_FOCUS.fig
%      MATLAB_APBS_FOCUS, by itself, creates a new MATLAB_APBS_FOCUS or raises the existing
%      singleton*.
%
%      H = MATLAB_APBS_FOCUS returns the handle to a new MATLAB_APBS_FOCUS or the handle to
%      the existing singleton*.
%
%      MATLAB_APBS_FOCUS('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in MATLAB_APBS_FOCUS.M with the given input arguments.
%
%      MATLAB_APBS_FOCUS('Property','Value',...) creates a new MATLAB_APBS_FOCUS or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before MATLAB_APBS_FOCUS_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to MATLAB_APBS_FOCUS_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help MATLAB_APBS_FOCUS

% Last Modified by GUIDE v2.5 13-Dec-2009 09:49:05

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @MATLAB_APBS_FOCUS_OpeningFcn, ...
                   'gui_OutputFcn',  @MATLAB_APBS_FOCUS_OutputFcn, ...
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


% --- Executes just before MATLAB_APBS_FOCUS is made visible.
function MATLAB_APBS_FOCUS_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to MATLAB_APBS_FOCUS (see VARARGIN)

% Choose default command line output for MATLAB_APBS_FOCUS
handles.output = hObject;
handles.dime1=0;
handles.dime2=0;
handles.dime3=0;
handles.glen1=0;
handles.glen2=0;
handles.glen3=0;
handles.temp=0;
handles.ionic=0;
handles.solvent=0;
handles.dielx='';
handles.diely='';
handles.dielz='';
handles.pathdielx='';
handles.kappa='';
handles.mol1='';
handles.mol2='';
handles.cout='';
handles.memorypath='.';
handles.ener='calcenerno';
handles.bc='sdh';
handles.digit=6;
handles.filename='focusname.inm';
% Update handles structure
guidata(hObject, handles);

% UIWAIT makes MATLAB_APBS_FOCUS wait for user response (see UIRESUME)
 %uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = MATLAB_APBS_FOCUS_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;



function edit2_Callback(hObject, eventdata, handles)
% hObject    handle to edit2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit2 as text
%        str2double(get(hObject,'String')) returns contents of edit2 as a double
handles.dime2=int16(str2double(get(hObject,'String')));
guidata(hObject,handles);

% --- Executes during object creation, after setting all properties.
function edit2_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit3_Callback(hObject, eventdata, handles)
% hObject    handle to edit3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit3 as text
%        str2double(get(hObject,'String')) returns contents of edit3 as a double
handles.dime3=int16(str2double(get(hObject,'String')));
guidata(hObject,handles);

% --- Executes during object creation, after setting all properties.
function edit3_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit4_Callback(hObject, eventdata, handles,glen1)
% hObject    handle to edit4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit4 as text
%        str2double(get(hObject,'String')) returns contents of edit4 as a double
handles.glen1=str2double(get(hObject,'String'));
guidata(hObject,handles);

% --- Executes during object creation, after setting all properties.
function edit4_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit5_Callback(hObject, eventdata, handles,glen2)
% hObject    handle to edit5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit5 as text
%        str2double(get(hObject,'String')) returns contents of edit5 as a double
handles.glen2=str2double(get(hObject,'String'));
guidata(hObject,handles);

% --- Executes during object creation, after setting all properties.
function edit5_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit6_Callback(hObject, eventdata, handles,glen3)
% hObject    handle to edit6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit6 as text
%        str2double(get(hObject,'String')) returns contents of edit6 as a double
handles.glen3=str2double(get(hObject,'String'));
guidata(hObject,handles);

% --- Executes during object creation, after setting all properties.
function edit6_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit7_Callback(hObject, eventdata, handles)
% hObject    handle to edit7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit7 as text
%        str2double(get(hObject,'String')) returns contents of edit7 as a double
%handles.dielx=get(hObject,'String');
%guidata(hObject,handles);
%
% if strcmp(handles.filename, 'focusname.inm')==1
% set(hObject, 'Enable','on');
% set(hObject, 'String','');
% handles.dielx='';
% guidata(hObject,handles);
% end
filespec = {'*.dx', 'Open Format files (*.dx)'};
[handles.dielx, handles.pathdielx] = uigetfile(filespec, 'Select the Required File',handles.memorypath);

if (ischar(handles.dielx)==0)
    handles.dielx='';
elseif strcmp(handles.dielx, '')==0
%     addpath(handles.pathdielx)
%     [handles.rmin,handles.dime,handles.h]=gridinf(handles.dielx);
handles.memorypath=handles.pathdielx;
end
set(hObject, 'String',handles.dielx);
guidata(hObject,handles);
% button = questdlg('Are you sure this is the correct file? You are not able to change it later!','Confirmation','Yes','No','Yes');
% if strcmp(button, 'Yes')==1
% set(hObject, 'Enable','off');
% else 
%     msgbox('please, press "enter" and try again, thanks','Confirmation','none');
%     set(hObject, 'String','');
%     guidata(hObject,handles);
% end
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



function edit10_Callback(hObject, eventdata, handles)
% hObject    handle to edit10 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit10 as text
%        str2double(get(hObject,'String')) returns contents of edit10 as a double

handles.temp=str2double(get(hObject,'String'));
guidata(hObject, handles);
% --- Executes during object creation, after setting all properties.
function edit10_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit10 (see GCBO)
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
%handles.diely=get(hObject,'String');
%guidata(hObject, handles);
%
filespec = {'*.dx', 'Open Format files (*.dx)'};
[handles.diely, handles.pathdielx] = uigetfile(filespec, 'Select the Required File',handles.memorypath);

if (ischar(handles.diely)==0)
    handles.diely='';
elseif strcmp(handles.diely, '')==0
%     addpath(handles.pathdielx)
%     [handles.rmin,handles.dime,handles.h]=gridinf(handles.dielx);
handles.memorypath=handles.pathdielx;
end
set(hObject, 'String',handles.diely);
guidata(hObject,handles);
% button = questdlg('Are you sure this is the correct file? You are not able to change it later!','Confirmation','Yes','No','Yes');
% if strcmp(button, 'Yes')==1
% set(hObject, 'Enable','off');
% else 
%     msgbox('please, press "enter" and try again, thanks','Confirmation','none');
%     set(hObject, 'String','');
%     guidata(hObject,handles);
% end

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
%handles.dielz=get(hObject,'String');
%guidata(hObject, handles);
%
filespec = {'*.dx', 'Open Format files (*.dx)'};
[handles.dielz, handles.pathdielx] = uigetfile(filespec, 'Select the Required File',handles.memorypath);

if (ischar(handles.dielz)==0)
    handles.dielz='';
elseif strcmp(handles.dielz, '')==0
%     addpath(handles.pathdielx)
%     [handles.rmin,handles.dime,handles.h]=gridinf(handles.dielx);
handles.memorypath=handles.pathdielx;
end
set(hObject, 'String',handles.dielz);
guidata(hObject,handles);
% button = questdlg('Are you sure this is the correct file? You are not able to change it later!','Confirmation','Yes','No','Yes');
% if strcmp(button, 'Yes')==1
% set(hObject, 'Enable','off');
% else 
%     msgbox('please, press "enter" and try again, thanks','Confirmation','none');
%     set(hObject, 'String','');
%     guidata(hObject,handles);
% end

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
% handles.kappa=get(hObject,'String');
% guidata(hObject, handles);
%
filespec = {'*.dx', 'Open Format files (*.dx)'};
[handles.kappa, handles.pathdielx] = uigetfile(filespec, 'Select the Required File',handles.memorypath);

if (ischar(handles.kappa)==0)
    handles.kappa='';
elseif strcmp(handles.kappa, '')==0
%     addpath(handles.pathdielx)
%     [handles.rmin,handles.dime,handles.h]=gridinf(handles.dielx);
handles.memorypath=handles.pathdielx;
end
set(hObject, 'String',handles.kappa);
guidata(hObject,handles);
% button = questdlg('Are you sure this is the correct file? You are not able to change it later!','Confirmation','Yes','No','Yes');
% if strcmp(button, 'Yes')==1
% set(hObject, 'Enable','off');
% else 
%     msgbox('please, press "enter" and try again, thanks','Confirmation','none');
%     set(hObject, 'String','');
%     guidata(hObject,handles);
% end

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



function edit15_Callback(hObject, eventdata, handles)
% hObject    handle to edit15 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit15 as text
%        str2double(get(hObject,'String')) returns contents of edit15 as a double
% handles.mol1=get(hObject,'String');
% guidata(hObject, handles);
%
filespec = {'*.pqr', 'Format File (*.pqr)'};
[handles.mol1, handles.pathdielx] = uigetfile(filespec, 'Select the Required File',handles.memorypath);

if (ischar(handles.mol1)==0)
    handles.mol1='';
elseif strcmp(handles.mol1, '')==0
%     addpath(handles.pathdielx)
%     [handles.rmin,handles.dime,handles.h]=gridinf(handles.dielx);
handles.memorypath=handles.pathdielx;
end
set(hObject, 'String',handles.mol1);
guidata(hObject,handles);
% button = questdlg('Are you sure this is the correct file? You are not able to change it later!','Confirmation','Yes','No','Yes');
% if strcmp(button, 'Yes')==1
% set(hObject, 'Enable','off');
% else 
%     msgbox('please, press "enter" and try again, thanks','Confirmation','none');
%     set(hObject, 'String','');
%     guidata(hObject,handles);
% end

% --- Executes during object creation, after setting all properties.
function edit15_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit15 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit16_Callback(hObject, eventdata, handles)
% hObject    handle to edit16 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit16 as text
%        str2double(get(hObject,'String')) returns contents of edit16 as a double

% handles.mol2=get(hObject,'String');
% guidata(hObject, handles);
%
filespec = {'*.pqr', 'Format File (*.pqr)'};
[handles.mol2, handles.pathdielx] = uigetfile(filespec, 'Select the Required File',handles.memorypath);

if (ischar(handles.mol2)==0)
    handles.mol2='';
elseif strcmp(handles.mol2, '')==0
%     addpath(handles.pathdielx)
%     [handles.rmin,handles.dime,handles.h]=gridinf(handles.dielx);
handles.memorypath=handles.pathdielx;
end
set(hObject, 'String',handles.mol2);
guidata(hObject,handles);
% button = questdlg('Are you sure this is the correct file? You are not able to change it later!','Confirmation','Yes','No','Yes');
% if strcmp(button, 'Yes')==1
% set(hObject, 'Enable','off');
% else 
%     msgbox('please, press "enter" and try again, thanks','Confirmation','none');
%     set(hObject, 'String','');
%     guidata(hObject,handles);
% end

% --- Executes during object creation, after setting all properties.
function edit16_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit16 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit17_Callback(hObject, eventdata, handles)
% hObject    handle to edit17 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit17 as text
%        str2double(get(hObject,'String')) returns contents of edit17 as a double
handles.digit=int8(str2double(get(hObject,'String')));
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function edit17_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit17 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit18_Callback(hObject, eventdata, handles)
% hObject    handle to edit18 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit18 as text
%        str2double(get(hObject,'String')) returns contents of edit18 as a double
handles.ionic=str2double(get(hObject,'String'));
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function edit18_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit18 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit19_Callback(hObject, eventdata, handles)
% hObject    handle to edit19 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit19 as text
%        str2double(get(hObject,'String')) returns contents of edit19 as a double
handles.solvent=str2double(get(hObject,'String'));
guidata(hObject, handles);

% --- Executes during object creation, after setting all properties.
function edit19_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit19 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function edit23_Callback(hObject, eventdata, handles)
% hObject    handle to edit23 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit23 as text
%        str2double(get(hObject,'String')) returns contents of edit23 as a double
% handles.cout=get(hObject,'String');
% guidata(hObject, handles);
%
handles.cout = uigetdir(handles.memorypath,'please, select Directory to Open');
if (ischar(handles.cout)==0)
    handles.cout='';
end
set(hObject, 'String',handles.cout);
%handles.cout=get(hObject,'String');
guidata(hObject,handles);
% button = questdlg('Are you sure this is the the correct Directory? You are not able to change it later!','Confirmation','Yes','No','Yes');
% if strcmp(button, 'Yes')==1
% set(hObject, 'Enable','off');
% else 
%     msgbox('please, press "Enter" and try again, thanks','Confirmation','none');
%         set(hObject, 'String','');
%     guidata(hObject,handles);
% end

% --- Executes during object creation, after setting all properties.
function edit23_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit23 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function edit24_Callback(hObject, eventdata, handles)
% hObject    handle to edit24 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit24 as text
%        str2double(get(hObject,'String')) returns contents of edit24 as a double
handles.dime1=int16(str2double(get(hObject,'String')));
guidata(hObject,handles);

% --- Executes during object creation, after setting all properties.
function edit24_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit24 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in radiobutton1.
function radiobutton1_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
%bt1=get(hObject,'Value')
if (get(hObject,'Value') == get(hObject,'Max'))
handles.bc='sdh';
guidata(hObject, handles);
else
    handles.bc='focusname.inm';
guidata(hObject, handles);
end
% Hint: get(hObject,'Value') returns toggle state of radiobutton1


% --- Executes on button press in radiobutton2.
function radiobutton2_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
%bt2=get(hObject,'Value')
if (get(hObject,'Value') == get(hObject,'Max'))
handles.bc='focusname.inm';
guidata(hObject, handles);
else
    handles.bc='sdh';
guidata(hObject, handles);
end
% Hint: get(hObject,'Value') returns toggle state of radiobutton2


% --- Executes on button press in radiobutton3.
function radiobutton3_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
%bt3=get(hObject,'Value')
if (get(hObject,'Value') == get(hObject,'Max'))
handles.ener='calceneryes';
guidata(hObject, handles);
else
handles.ener='calcenerno';
guidata(hObject, handles);
end
% Hint: get(hObject,'Value') returns toggle state of radiobutton3


% --- Executes on button press in radiobutton4.
function radiobutton4_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
%bt4=get(hObject,'Value')
if (get(hObject,'Value') == get(hObject,'Max'))
handles.ener='calcenerno';
guidata(hObject, handles);
else
 handles.ener='calceneryes';
guidata(hObject, handles);
end
% Hint: get(hObject,'Value') returns toggle state of radiobutton4

% --- Executes on button press in pushbutton2.
function pushbutton2_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
%data=guidata(hOject);
dime(1)=handles.dime1;
dime(2)=handles.dime2;
dime(3)=handles.dime3;
glen(1)=handles.glen1;
glen(2)=handles.glen2;
glen(3)=handles.glen3;
bulk(1)=handles.ionic;
bulk(2)=handles.solvent;
fid = fopen(handles.filename, 'wt');
fprintf(fid,'%g %g %g\n',dime);
fprintf(fid,'%f %f %f\n', glen);
fprintf(fid,'%f\n', handles.temp);
fprintf(fid,'%f %f\n', bulk);
fprintf(fid,'%s\n',handles.bc);
fprintf(fid,'%g\n',handles.digit);
fprintf(fid,'%s\n',handles.dielx);
fprintf(fid,'%s\n',handles.diely);
fprintf(fid,'%s\n',handles.dielz);
fprintf(fid,'%s\n',handles.kappa);
fprintf(fid,'%s\n',handles.mol1);
fprintf(fid,'%s\n',handles.mol2);
fprintf(fid,'%s\n',handles.ener);
fprintf(fid,'%s\n',handles.pathdielx);
fprintf(fid,'%s\n',handles.cout);
fclose(fid);
%close
 button = questdlg('Are you sure you provided the correct information? You wont be able to change it later!','Confirmation','Yes','No','Yes');
 if strcmp(button, 'No')==1
% set(hObject, 'Enable','off');
% else 
     msgbox('Please, make all the neccessary corrections and then press "Generate Input Files" again, thanks.!!','Confirmation','none');
%     set(hObject, 'String','');
%     guidata(hObject,handles);
 else
thi=msgbox('The focusname.inm was successfully generated. Please, press Run MAPBS','!!GREAT!!','none');
waitfor(thi)
 end


% --- Executes on button press in pushbutton3.
function pushbutton3_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
close
th=msgbox('The MAPBS will run in your MATLAB command window. you can close any other matlab windows anytime.','MAPBS Calculation','none','modal');
waitfor(th)
clear
clc
 MAPBS('inputfile.inm')
 



