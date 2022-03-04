root:
  mesh:
    scale:
      x: 32
      y: 32
    material: Assets1/Materials/TestBody
    data: Assets1/Mesh/Sprite
  children:
    head:
      offset:
        y: 16
        z: 1
      mesh:
        scale:
          x: 32
          y: 32
        material: Assets1/Materials/TestHead
        data: Assets1/Mesh/Sprite
    left_shoulder:
      offset:
        x: 16
        y: 0
        z: 2
      rotation:
        z: 5
      mesh:
        scale:
          x: 16
          y: 16
        material: Assets1/Materials/TestHand
        data: Assets1/Mesh/Sprite
    right_hand:
      offset:
        x: -16
        y: 0
        z: 2
      rotation:
        z: -5
      mesh:
        scale:
          x: 16
          y: 16
        material: Assets1/Materials/TestHand
        data: Assets1/Mesh/Sprite
      children:
        weapon_attach_right:
          offset:
            z: 1