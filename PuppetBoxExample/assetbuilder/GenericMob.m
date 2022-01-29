root:
  scale:
    x: 32
    y: 32
  mesh:
    type: sprite
    offset:
      x: -16
      y: 16
      z: 0
    material: Assets1/Materials/TestBody
  children:
    head:
      offset:
        x: 0
        y: 16 
        z: -1
      scale:
        x: 32
        y: 32
      mesh:
        type: sprite
        offset:
          x: -16
          y: 16
        material: Assets1/Materials/TestHead
    left_hand:
      offset:
        x: 16
        y: 0
        z: -2
      scale:
        x: 16
        y: 16
      mesh:
        type: sprite
        offset:
          x: -8
          y: 8
        material: Assets1/Materials/TestHand
    right_hand:
      offset:
        x: -16
        y: 0
        z: -2
      scale:
        x: 16
        y: 16
      mesh:
        type: sprite
        offset:
          x: -8
          y: 8
        material: Assets1/Materials/TestHand