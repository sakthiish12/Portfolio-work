//using System.Collections;
//using System.Collections.Generic;
//using UnityEngine;

//public class PlayerAimWeapon : MonoBehaviour
//{
//    private Transform aimTransform;
//    public bool hasWeapon;
//    public PistolControl Gun1;


//     void Awake()
//    {
//        aimTransform = transform.Find("Aim"); //This looks for the gameobject called Aim

//    }


//    public void Start()
//    {
//        hasWeapon = true;
//    }

//    //Get Mouse Position in world with Z Axis = Of
//    public static Vector3 GetMouseWorldPosition()
//    {
//        Vector3 vec = GetMouseWorldPositionWithZ(Input.mousePosition, Camera.main);
//        vec.z = 0f;
//        return vec;
//    }
//    public static Vector3 GetMouseWorldPositionWithZ()
//    {
//        return GetMouseWorldPositionWithZ(Input.mousePosition, Camera.main);
//    }
//    public static Vector3 GetMouseWorldPositionWithZ(Camera worldCamera)
//    {
//        return GetMouseWorldPositionWithZ(Input.mousePosition, worldCamera);
//    }

//    public static Vector3 GetMouseWorldPositionWithZ(Vector3 screenPosition, Camera worldCamera)
//    {
//        Vector3 worldPosition = worldCamera.ScreenToWorldPoint(screenPosition);
//        return worldPosition;
//    }
 
//    private void Update()
//    {
//        HandleAiming();
//        HandleShooting();
//    }

//    private void HandleAiming()
//    {
//        Vector3 mousePosition = GetMouseWorldPosition();
//        Vector3 aimDirection = (mousePosition - transform.position).normalized;
//        float angle = Mathf.Atan2(aimDirection.y, aimDirection.x) * Mathf.Rad2Deg;
//        aimTransform.eulerAngles = new Vector3(0, 0, angle);

//        Vector3 localScale = Vector3.one;
//        if (angle > 90 || angle <-90)
//        {
//            localScale.y = -1f;
//        }

//        else
//        {
//            localScale.y = +1f;
//        }
//        aimTransform.localScale = localScale;

       
//    }

//    private void HandleShooting()
//    {
//        if (Input.GetMouseButtonDown(0))
//            {

//            if (hasWeapon == true)
//            {
//                Debug.Log("Fire");
//                Gun1.Shooting();
//            }



//        }
//    }
//}
