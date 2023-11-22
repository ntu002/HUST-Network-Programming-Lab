import pygame
from pygame.locals import *

# Khởi tạo pygame
pygame.init()

# Cài đặt màn hình
screen_width, screen_height = 800, 600
screen = pygame.display.set_mode((screen_width, screen_height))
pygame.display.set_caption('Ping Pong Game')

# Màu sắc
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)

# Thanh trượt và bóng
paddle_width, paddle_height = 15, 90
ball_size = 20

# Vị trí ban đầu
paddle1_x, paddle1_y = 20, (screen_height - paddle_height) // 2
paddle2_x, paddle2_y = screen_width - 20 - paddle_width, (screen_height - paddle_height) // 2
ball_x, ball_y = (screen_width - ball_size) // 2, (screen_height - ball_size) // 2

# Tốc độ
ball_speed_x, ball_speed_y = 7, 7
paddle_speed = 7

# Chạy trò chơi
running = True
while running:
    for event in pygame.event.get():
        if event.type == QUIT:
            running = False

    # Di chuyển bóng
    ball_x += ball_speed_x
    ball_y += ball_speed_y

    # Kiểm tra va chạm với tường
    if ball_y <= 0 or ball_y >= screen_height - ball_size:
        ball_speed_y *= -1

    # Vẽ mọi thứ
    screen.fill(BLACK)
    paddle1 = pygame.draw.rect(screen, WHITE, (paddle1_x, paddle1_y, paddle_width, paddle_height))
    paddle2 = pygame.draw.rect(screen, WHITE, (paddle2_x, paddle2_y, paddle_width, paddle_height))
    ball = pygame.draw.rect(screen, WHITE, (ball_x, ball_y, ball_size, ball_size))

    pygame.display.flip()
    pygame.time.Clock().tick(60)

pygame.quit()
