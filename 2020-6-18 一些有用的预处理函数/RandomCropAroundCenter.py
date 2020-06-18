import numpy as np

class RandomCropAroundCenter(object):
    def __call__(self, image, mask):
        assert mask.ndim == 2 and image.ndim == 3

        H,W,_   = image.shape
        center_h, center_w = H//2, W//2

        w_index, h_index = np.meshgrid(np.arange(W), np.arange(H))
        object_w_r = w_index[mask > 0].max() + 1
        object_w_l = w_index[mask > 0].min()
        object_h_d = h_index[mask > 0].max() + 1
        object_h_t = h_index[mask > 0].min()
        object_h = object_h_d - object_h_t
        object_w = object_w_r - object_w_l

        double_object_h = 2 * object_h
        double_object_w = 2 * object_w
        start_h = np.random.randint(low=max(center_h - object_h, 0), high=center_h)
        start_w = np.random.randint(low=max(center_w - object_w, 0), high=center_w)

        padding_h = center_h + double_object_h - H if center_h + double_object_h > H else 0
        padding_w = center_w + double_object_w - W if center_w + double_object_w > W else 0
        if padding_h > 0 or padding_w > 0:
            image_pad = np.pad(
                image,
                pad_width=((0, padding_h), (0, padding_w), (0, 0)),
                mode='constant',
                constant_values=0
            )
            mask_pad = np.pad(
                mask,
                pad_width=((0, padding_h), (0, padding_w)),
                mode='constant',
                constant_values=0
            )
        else:
            image_pad = image
            mask_pad = mask

        image_crop = image_pad[start_h: start_h + double_object_h, start_w: start_w + double_object_w]
        mask_crop = mask_pad[start_h: start_h + double_object_h, start_w: start_w + double_object_w]
        return image_crop, mask_crop
