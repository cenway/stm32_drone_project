% 시리얼 포트 열기 (COM 번호는 장치관리자에서 확인)
s = serialport("COM3", 115200);
configureTerminator(s, "CR/LF");
flush(s);
% 드론 형태 꼭짓점 정의 (십자 모양)
drone_vertices = [
    -8  -1  0;   8  -1  0;   8   1  0;  -8   1  0;   % arm 1
    -1  -8  0;   1  -8  0;   1   8  0;  -1   8  0;   % arm 2
    -8  -1  0.5; 8  -1  0.5; 8   1  0.5;-8   1  0.5;
    -1  -8  0.5; 1  -8  0.5; 1   8  0.5;-1   8  0.5;
];

drone_faces = [
    1 2 3 4; 5 6 7 8;               % 하판
    9 10 11 12; 13 14 15 16;         % 상판
    1 2 10 9; 2 3 11 10;             % arm1 측면
    3 4 12 11; 4 1 9 12;
    5 6 14 13; 6 7 15 14;            % arm2 측면
    7 8 16 15; 8 5 13 16;
];

figure('Name', 'Drone Attitude', 'NumberTitle', 'off');

% 3D 뷰
ax3d = subplot(2,1,1);
h = patch('Vertices', drone_vertices, 'Faces', drone_faces, ...
    'FaceColor', [0.2 0.6 1], 'EdgeColor', 'k', 'FaceAlpha', 0.8);
axis equal;
xlim([-15 15]); ylim([-15 15]); zlim([-10 10]);
grid on; view(30, 25);
xlabel('X'); ylabel('Y'); zlabel('Z');
title('Drone 3D Attitude');

% 2D 그래프
ax2d = subplot(2,1,2);
max_points = 200;
roll_buf  = zeros(1, max_points);
pitch_buf = zeros(1, max_points);
yaw_buf   = zeros(1, max_points);
h_roll  = plot(ax2d, roll_buf,  'r', 'LineWidth', 1.5); hold on;
h_pitch = plot(ax2d, pitch_buf, 'g', 'LineWidth', 1.5);
h_yaw   = plot(ax2d, yaw_buf,  'b', 'LineWidth', 1.5);
legend('Roll', 'Pitch', 'Yaw');
ylim([-180 180]); grid on;
title('Euler Angles (deg)');
ylabel('Degrees');

% 실시간 루프
while isvalid(ax3d)
    line = readline(s);
    data = str2double(split(line, ','));

    if numel(data) == 3 && ~any(isnan(data))
        roll_deg  = data(1);
        pitch_deg = data(2);
        yaw_deg   = data(3);

        % 회전 행렬 (ZYX 순서)
        R = eul2rotm(deg2rad([yaw_deg pitch_deg roll_deg]), 'ZYX');

        % 꼭짓점 회전 적용
        rotated = (R * drone_vertices')';
        set(h, 'Vertices', rotated);
        title(ax3d, sprintf('Roll: %.1f°  Pitch: %.1f°  Yaw: %.1f°', ...
            roll_deg, pitch_deg, yaw_deg));

        % 2D 버퍼 업데이트
        roll_buf  = [roll_buf(2:end),  roll_deg];
        pitch_buf = [pitch_buf(2:end), pitch_deg];
        yaw_buf   = [yaw_buf(2:end),   yaw_deg];
        set(h_roll,  'YData', roll_buf);
        set(h_pitch, 'YData', pitch_buf);
        set(h_yaw,   'YData', yaw_buf);

        drawnow limitrate;
    end
end

clear s;