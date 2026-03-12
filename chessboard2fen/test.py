import os
import sys
import urllib.request
import numpy as np
import cv2
os.environ["TF_USE_LEGACY_KERAS"] = "1"
from boardDetection import ChessboardDetector

# Lấy đường dẫn tuyệt đối của thư mục chứa file test.py này
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# Từ điển dịch mã quân cờ từ tiếng Đức của tác giả sang ký hiệu FEN chuẩn
FEN_MAP = {
    0: 'p', 1: 'P',   # Bauer (Tốt)
    2: 'q', 3: 'Q',   # Dame (Hậu)
    4: 'k', 5: 'K',   # Koenig (Vua)
    6: '1',           # LEER (Ô trống)
    7: 'b', 8: 'B',   # Laeufer (Tượng)
    9: 'n', 10: 'N',  # Pferd (Mã)
    11: 'r', 12: 'R'  # Turm (Xe)
}

def generate_fen(predictions):
    """Chuyển đổi mảng 64 số thành chuỗi FEN chuẩn"""
    
    # BƯỚC QUAN TRỌNG: Chuyển mảng 1D thành 2D 8x8, sau đó xoay 90 độ cho đúng chiều FEN
    # Logic này được trích xuất từ chính mã nguồn gốc của tác giả repo
    rotated_predictions = np.rot90(np.array(predictions).reshape((8, 8)), k=1, axes=(1, 0)).flatten()
    
    fen_rows = []
    for row in range(8):
        empty_count = 0
        row_str = ""
        for col in range(8):
            # Lấy dữ liệu từ mảng ĐÃ ĐƯỢC XOAY
            piece_idx = rotated_predictions[row * 8 + col]
            
            if piece_idx == 6:  # Mã 6 là ô trống (LEER)
                empty_count += 1
            else:
                if empty_count > 0:
                    row_str += str(empty_count)
                    empty_count = 0
                row_str += FEN_MAP[piece_idx]
                
        if empty_count > 0:
            row_str += str(empty_count)
            
        fen_rows.append(row_str)
    
    # Nối 8 hàng lại với nhau, thêm lượt đi mặc định cho quân Trắng
    return "/".join(fen_rows) + " w - - 0 1"

def url_to_image(url):
    """Tải ảnh từ URL và chuyển thành mảng numpy cho OpenCV"""
    # Thêm User-Agent để tránh bị một số trang web chặn bot
    req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})
    resp = urllib.request.urlopen(req)
    image = np.asarray(bytearray(resp.read()), dtype="uint8")
    image = cv2.imdecode(image, cv2.IMREAD_COLOR)
    return image

def load_image(source):
    """Kiem tra nguon la URL hay file cuc bo"""
    # Neu bat dau bang http thi la URL
    if source.startswith(('http://', 'https://')):
        return url_to_image(source)
    else:
        # Neu la duong dan file trong may
        # Dung numpy de doc de tranh loi voi duong dan co dau tieng Viet
        img = cv2.imdecode(np.fromfile(source, dtype=np.uint8), cv2.IMREAD_COLOR)
        return img

def main():
    # Kiểm tra xem có URL được truyền vào từ C++ không
    if len(sys.argv) < 2:
        print("LỖI: Vui lòng cung cấp URL của hình ảnh.")
        return

    image_url = sys.argv[1]
    
    try:
        detect_path = os.path.join(BASE_DIR, "models", "detection")
        class_path = os.path.join(BASE_DIR, "models", "classification.h5")
        
        detector = ChessboardDetector(detect_path, class_path)
        img = load_image(image_url)
        
        if img is None:
            print("LỖI: Không thể tải hoặc đọc ảnh từ URL này.")
            return

        corners = detector.predict_board_corners(img)
        if len(corners) == 0:
            print("LỖI: Không tìm thấy bàn cờ.")
            return

        predictions = detector.predictBoard(img, corners)
        final_fen = generate_fen(predictions)
        
        # QUAN TRỌNG: Chỉ in ra duy nhất chuỗi FEN để C++ dễ dàng đọc được
        # (Bạn nên xóa hoặc comment các dòng print("1. Dang khoi tao...") cũ đi)
        print(f"FEN_RESULT:{final_fen}")
        
    except Exception as e:
        print(f"PYTHON_ERROR: {str(e)}")
        
if __name__ == "__main__":
    main()