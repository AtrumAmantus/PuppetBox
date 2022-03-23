root:
  mesh:
    scale:
      x: 32
      y: 32
    material: Assets1/Materials/TestBody
    data: Default/Mesh/Sprite
  children:
    collar:
      offset:
        y: 16
        z: 1
      children:
        neck:
          offset:
            z: -10
          children:
            head:
              offset:
                z: 10
              mesh:
                scale:
                  x: 32
                  y: 32
                material: Assets1/Materials/TestHead
                data: Default/Mesh/Sprite
    left_shoulder:
      offset:
        x: 16
        z: 2
      children:
        left_elbow:
          offset:
            y: -16
          children:
            left_hand:
              offset:
                y: 16
              mesh:
                scale:
                  x: 16
                  y: 16
                material: Assets1/Materials/TestHand
                data: Default/Mesh/Sprite
    right_shoulder:
      offset:
        x: -16
        z: 2
      children:
        right_elbow:
          offset:
            y: -16
          children:
            right_hand:
              offset:
                y: 16
              mesh:
                scale:
                  x: 16
                  y: 16
                material: Assets1/Materials/TestHand
                data: Default/Mesh/Sprite
              children:
                weapon_attach_right:
                  offset:
                    z: 1