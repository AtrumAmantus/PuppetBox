root:
  mesh:
    type: sprite
    offset:
      x: 0
      y: 0
      z: 0
    scale:
      x: 32
      y: 32
    material: Assets1/Materials/TestBody
  children:
    head:
      offset:
        x: 0
        y: 16
        z: -1
      mesh:
        type: sprite
        offset:
          x: 0
          y: 0
        scale:
          x: 32
          y: 32
        material: Assets1/Materials/TestHead
    left_hand:
      offset:
        x: 16
        y: 0
        z: -2
      rotation:
        z: 5
      mesh:
        type: sprite
        offset:
          x: 0
          y: 0
        scale:
          x: 16
          y: 16
        material: Assets1/Materials/TestHand
    right_hand:
      offset:
        x: -16
        y: 0
        z: -2
      rotation:
        z: -5
      mesh:
        type: sprite
        offset:
          x: 0
          y: 0
        scale:
          x: 16
          y: 16
        material: Assets1/Materials/TestHand