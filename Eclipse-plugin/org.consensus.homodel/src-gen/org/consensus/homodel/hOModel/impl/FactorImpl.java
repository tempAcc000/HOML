/**
 * generated by Xtext 2.26.0
 */
package org.consensus.homodel.hOModel.impl;

import java.util.Collection;

import org.consensus.homodel.hOModel.Expression;
import org.consensus.homodel.hOModel.Factor;
import org.consensus.homodel.hOModel.HOModelPackage;
import org.consensus.homodel.hOModel.Variable;

import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Factor</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * </p>
 * <ul>
 *   <li>{@link org.consensus.homodel.hOModel.impl.FactorImpl#getExp <em>Exp</em>}</li>
 *   <li>{@link org.consensus.homodel.hOModel.impl.FactorImpl#getVar <em>Var</em>}</li>
 * </ul>
 *
 * @generated
 */
public class FactorImpl extends MinimalEObjectImpl.Container implements Factor
{
  /**
   * The cached value of the '{@link #getExp() <em>Exp</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getExp()
   * @generated
   * @ordered
   */
  protected EList<Expression> exp;

  /**
   * The cached value of the '{@link #getVar() <em>Var</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @see #getVar()
   * @generated
   * @ordered
   */
  protected EList<Variable> var;

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  protected FactorImpl()
  {
    super();
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  protected EClass eStaticClass()
  {
    return HOModelPackage.Literals.FACTOR;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public EList<Expression> getExp()
  {
    if (exp == null)
    {
      exp = new EObjectContainmentEList<Expression>(Expression.class, this, HOModelPackage.FACTOR__EXP);
    }
    return exp;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public EList<Variable> getVar()
  {
    if (var == null)
    {
      var = new EObjectContainmentEList<Variable>(Variable.class, this, HOModelPackage.FACTOR__VAR);
    }
    return var;
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs)
  {
    switch (featureID)
    {
      case HOModelPackage.FACTOR__EXP:
        return ((InternalEList<?>)getExp()).basicRemove(otherEnd, msgs);
      case HOModelPackage.FACTOR__VAR:
        return ((InternalEList<?>)getVar()).basicRemove(otherEnd, msgs);
    }
    return super.eInverseRemove(otherEnd, featureID, msgs);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public Object eGet(int featureID, boolean resolve, boolean coreType)
  {
    switch (featureID)
    {
      case HOModelPackage.FACTOR__EXP:
        return getExp();
      case HOModelPackage.FACTOR__VAR:
        return getVar();
    }
    return super.eGet(featureID, resolve, coreType);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @SuppressWarnings("unchecked")
  @Override
  public void eSet(int featureID, Object newValue)
  {
    switch (featureID)
    {
      case HOModelPackage.FACTOR__EXP:
        getExp().clear();
        getExp().addAll((Collection<? extends Expression>)newValue);
        return;
      case HOModelPackage.FACTOR__VAR:
        getVar().clear();
        getVar().addAll((Collection<? extends Variable>)newValue);
        return;
    }
    super.eSet(featureID, newValue);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public void eUnset(int featureID)
  {
    switch (featureID)
    {
      case HOModelPackage.FACTOR__EXP:
        getExp().clear();
        return;
      case HOModelPackage.FACTOR__VAR:
        getVar().clear();
        return;
    }
    super.eUnset(featureID);
  }

  /**
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   * @generated
   */
  @Override
  public boolean eIsSet(int featureID)
  {
    switch (featureID)
    {
      case HOModelPackage.FACTOR__EXP:
        return exp != null && !exp.isEmpty();
      case HOModelPackage.FACTOR__VAR:
        return var != null && !var.isEmpty();
    }
    return super.eIsSet(featureID);
  }

} //FactorImpl
